#include "stdafx.h"
#include "GameMap.h"

#include "Client.h"
#include "MainWindow.h"
#include "Battle.h"
#include "BattleResult.h"
#include "GameLibrary.h"
#include "CustomEvents.h"

#include "Player.h"
#include "User.h"
#include "NPC.h"

#include "UserManager.h"
#include "AssetManager.h"

#include "pNpc.h"
#include "pNpcAction.h"
#include "pAction.h"
#include "pDirection.h"

#include "ShopForm.h"
#include "Dialogue.h"


GameMap::GameMap(pMapInfo* packet) {
	renderer = mainForm->renderer;

	id = packet->mapId;
	doc = packet->mapDoc;
	type = packet->mapType;
	weather = packet->weather;
	bgMusic = packet->bgMusic;
	bgMusic_show = packet->bgMusic_show;
	fightMusic = packet->fightMusic;

	name = packet->mapName;

	cameraX = 0; mouseX = 0;
	cameraY = 0; mouseY = 0;
	renderRect = { 0, 0, uiRect.w, uiRect.h };

	loadMapFile(doc);
}

GameMap::~GameMap() {
	SDL_DestroyTexture(tileTexture); tileTexture = NULL;

	if (mapCoordinates) delete[] mapCoordinates;

	/*for (int i = masks.size(); i > 0; i--) {
		masks.at(i - 1)->deleteTextures();
		//delete masks.at(i - 1); //duplicated in objects
	}
	masks.clear();*/

	for (auto obj : mapObjects)
		delete obj;
	mapObjects.clear();

	for (auto npc : npcs)
		delete npc;
	npcs.clear();

	assetManager.releaseAssets(this);
}

void GameMap::loadMapFile(int mapDoc) {
	MapFile mapFile(doc);

	//Load the map image from file
	std::string mapPath = mapFile.imagePath;
	for (int i = 0; i < mapPath.size(); i++) { if (mapPath.at(i) == '/') mapPath.at(i) = '\\'; }
	tMap.reset(new Texture(renderer, mapPath, true));
	assetManager.addAndQueueAsset(this, tMap);

	//Load map object masks
	if (mapFile.masks.size()) {
		for (int i = 0; i < mapFile.masks.size(); i++) {
			masks.push_back(mapFile.masks.at(i).file);
			std::vector<Asset> assetVector;
			INI aniIni("INI\\ani.ini", mapFile.masks.at(i).file);
			if (aniIni.currentSection >= 0) {
				Texture* aMask;

				int frameCount = std::stoi(aniIni.getEntry("FrameAmount"));
				for (int j = 0; j < frameCount; j++) {
					std::string nextFrame = "Frame" + std::to_string(j);
					std::string aFrame = aniIni.getEntry(nextFrame);

					aMask = new Texture(renderer, aFrame, false);
					aMask->setPosition(SDL_Point{ -mapFile.masks.at(i).xOffset, -mapFile.masks.at(i).yOffset });
					Asset mAsset(aMask);

					assetManager.addAndQueueAsset(this, mAsset);
					assetVector.push_back(mAsset);
				}
			}
			mapAssets.push_back(assetVector);
		}
	}

	//Load map attributes
	mapWidth = mapFile.width;
	mapHeight = mapFile.height;
	mapOffsetX = 0;
	mapOffsetY = 0;
	if (tMap->width < uiRect.w) mapOffsetX = (uiRect.w / 2) - (tMap->width / 2);
	if (tMap->height < uiRect.h) mapOffsetY = (uiRect.h / 2) - (tMap->height / 2);

	mapCoordinates = new char[mapWidth * mapHeight];
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			int pos = (y*mapWidth) + x;
			mapCoordinates[pos] = mapFile.coordinates[pos];
		}
	}

	//Create Portal texture
	{
		tileTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tileWidth, tileHeight);
		tileRect = { 0, 0, tileWidth, tileHeight };
		SDL_SetRenderTarget(renderer, tileTexture);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(tileTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderFillRect(renderer, NULL);
		SDL_RenderClear(renderer);

		Sint16 tX[4], tY[4];
		tX[0] = tileWidth / 2;	tY[0] = 0;
		tX[1] = tileWidth;		tY[1] = tileHeight / 2;
		tX[2] = tileWidth / 2;	tY[2] = tileHeight;
		tX[3] = 0;				tY[3] = tileHeight / 2;
		filledPolygonColor(renderer, tX, tY, 4, (Uint32)0x40FFFFFF);
		SDL_SetRenderTarget(renderer, NULL);
	}


	//This sometimes results in a hanging operation. 
	//loaded is true, but it never triggers the break
	//to ivnestigate, set to default load instead
	/*while (true) {
		if (tMap->loaded) break;
	}*/
	tMap->Load(); //Once the surface is loaded, manually trigger loadind of the texture

	//Copy map to make it a renderable target
	SDL_Texture* tempTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tMap->width, tMap->height);
	SDL_SetRenderTarget(renderer, tempTexture);
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, tMap->texture, NULL, NULL);
	SDL_SetRenderTarget(renderer, NULL);
	SDL_DestroyTexture(tMap->texture);
	tMap->texture = tempTexture;

	//Draw portals on map texture
	if (mapFile.portals.size()) {
		portals = mapFile.portals;
		SDL_SetRenderTarget(renderer, tMap->texture);
		for (int i = 0; i < mapFile.portals.size(); i++) {
			SDL_Point pPoint = CoordToPoint(mapFile.portals.at(i).x, mapFile.portals.at(i).y);
			tileRect.x = pPoint.x;
			tileRect.y = pPoint.y;
			SDL_RenderCopy(renderer, tileTexture, NULL, &tileRect);
		}
		SDL_SetRenderTarget(renderer, NULL);
	}

	//Load map Objects as Sprites 
	// no longer do: Draw static instances of masks
	if (mapFile.objects.size()) {
		SDL_SetRenderTarget(renderer, tMap->texture);
		for (int i = 0; i < mapFile.objects.size(); i++) {
			int maskId = mapFile.objects.at(i).id;
			std::vector<Asset> vMask;
			Sprite* mSprite;

			vMask = mapAssets[maskId];
			if (vMask.size() > 0) {
				GameObj *obj = new GameObj(renderer);
				obj->SetCoord(SDL_Point{ mapFile.objects.at(i).x, mapFile.objects.at(i).y });
				obj->SetPosition(CenterOfCoord(mapFile.objects.at(i).x, mapFile.objects.at(i).y));

				int sType;
				if (vMask.size() > 1) sType = stObject;
				else sType = stStatic;
				//SDL_Point pPoint = CenterOfCoord(mapFile.objects.at(i).x, mapFile.objects.at(i).y);

				mSprite = new Sprite(renderer, vMask, sType);
				mSprite->setLocation(obj->GetPosition());
				mSprite->start();
				obj->SetSprite(mSprite);
				
				mapObjects.push_back(obj);

				mSprite->LoadFirst();
				/*if (vMask.size() == 1) {
					mSprite->render();
				}*/
			}
		}
		SDL_SetRenderTarget(renderer, NULL);
	}

	//Setup tile zone trigon array for map coordinate calculations
	/*SDL_Point a1 = { 0,0 }, a2 = { tileWidth / 2, 0 }, a3 = { 0, tileHeight / 2 };
	SDL_Point b1 = { tileWidth/2,0 }, b2 = { tileWidth, 0 }, b3 = { tileWidth, tileHeight / 2 };
	SDL_Point c1 = { 0,tileHeight / 2 }, c2 = { 0 , tileHeight }, c3 = { tileWidth / 2, tileHeight  };
	SDL_Point d1 = { tileWidth / 2,tileHeight }, d2 = { tileWidth , tileHeight }, d3 = { tileWidth, tileHeight / 2 };
	for (int i = 0; i < tileWidth; i++) {
	for (int j = 0; j < tileHeight; j++) {
	if (isPointInTrigon(i, j, a1, a2, a3)) tileZone[i][j] = TileZone::TOPLEFT;
	else if (isPointInTrigon(i, j, b1, b2, b3)) tileZone[i][j] = TileZone::TOPRIGHT;
	else if (isPointInTrigon(i, j, c1, c2, c3)) tileZone[i][j] = TileZone::BOTTOMLEFT;
	else if (isPointInTrigon(i, j, d1, d2, d3)) tileZone[i][j] = TileZone::BOTTOMRIGHT;
	}
	}*/
}

void GameMap::ReloadAssets() {
	tMap->Reload();
}

void GameMap::addNpc(pNpcInfo* packet) {
	NPC* newNpc = new NPC(packet);
	npcs.push_back(newNpc);
}


void GameMap::addColosseum(pColosseum* packet) {
	colosseum = new Colosseum(packet);
}


bool GameMap::handleEvent(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	if (e.type == SDL_MOUSEMOTION) {
		if (doesPointIntersect(uiRect, mx, my)) {
			OnMouseMove(e);
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		//mapClicked_left = false;
		//mapClicked_right = false;

		if (doesPointIntersect(uiRect, mx, my)) {
			//OnClick(e);
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (doesPointIntersect(uiRect, mx, my)) {
			//if (e.button.button == SDL_BUTTON_LEFT) mapClicked_left = true;
			//if (e.button.button == SDL_BUTTON_RIGHT) mapClicked_right = true;
			OnClick(e);
		}
	}

	// Custom Events

	if (e.type == CUSTOMEVENT_NPC) {
		NPC* sourceNPC = (NPC*)e.user.data1;

		if (e.user.code == NPC_INTERACT) dialogueNpc = sourceNPC;
		if (e.user.code == NPC_SHOP) {
			CShopForm *shopForm = new CShopForm();

			CShop *shop = (CShop*)e.user.data2;
			if (shop) {
				shopForm->SetShop(shop);
				delete shop;
			}
			Windows.push_back(shopForm);
		}
	}

	return false;
}


void GameMap::OnClick(SDL_Event& e) {
	int mx, my, cx, cy;
	SDL_GetMouseState(&mx, &my);
	mx -= uiRect.x + mapOffsetX;
	my -= uiRect.y + mapOffsetY;
	cx = mx + cameraX;
	cy = my + cameraY;

	setMouseCoordinates(mx, my);
	if (e.button.button == SDL_BUTTON_LEFT) {
		if (!changingMap) {
			if (jumpMode) {
				//Jump
				player->jumpTo(SDL_Point{ mouseX, mouseY });
				checkPortal = true;
			}
			else {
				//Walk
				player->walkTo(SDL_Point{ mouseX, mouseY });
			}
		}

		//Update mouse coordinates based on new positions
		setMouseCoordinates(mx, my);
		mainForm->setMapCoordLabels(SDL_Point{ mouseX, mouseY });
	}

	if (e.button.button == SDL_BUTTON_RIGHT) {
		SDL_Point toCoord{ mouseX, mouseY };
		int rcDir = player->getDirectionToCoord(toCoord);
		player->setDirectionToCoord(toCoord);
		player->loadSprite();

		pDirection *dirPacket = new pDirection(player->GetID(), player->GetCoord().x, player->GetCoord().y, rcDir);
		gClient.addPacket(dirPacket);

		SDL_Event e2 = e;
		e2.motion.x = cx;
		e2.motion.y = cy;
		for (auto npc : npcs) npc->handleEvent(e2);
		userManager.handleEventAllUsers(e2);
	}
}

void GameMap::OnMouseMove(SDL_Event& e) {
	int mx, my, cx, cy;
	SDL_GetMouseState(&mx, &my);
	mx -= uiRect.x + mapOffsetX;
	my -= uiRect.y + mapOffsetY;
	cx = mx + cameraX;
	cy = my + cameraY;

	//Update Mouse Coordinate
	setMouseCoordinates(mx, my);
	mainForm->setMapCoordLabels(SDL_Point{ mouseX, mouseY });

	//Process NPCs for mouseover
	SDL_Event e2 = e;
	e2.motion.x = cx;
	e2.motion.y = cy;
	for (auto npc : npcs) npc->handleEvent(e2);
	userManager.handleEventAllUsers(e2);
}


void GameMap::render() {
	SDL_Texture *origTarget = SDL_GetRenderTarget(renderer);

	//SDL_Texture* final_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, uiRect.w, uiRect.h);
	//SDL_SetRenderTarget(renderer, final_map);
	//SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
	//SDL_RenderClear(renderer);

	//assume pox x/y is already adjust to min of 0 and max of size-viewable width
	//this will be handled by set map pos function
	mapRect.x = cameraX;
	mapRect.y = cameraY;
	if (tMap->width < uiRect.w) mapRect.w = tMap->width;
	else mapRect.w = uiRect.w;
	if (tMap->height < uiRect.h) mapRect.h = tMap->height;
	else mapRect.h = uiRect.h;

	//If map doesn't reach width/hight, center it
	SDL_Rect finalRect;
	int finalX;
	if (tMap->width < uiRect.w) {
		finalRect.x = mapOffsetX;
		finalRect.w = tMap->width;
	}
	else {
		finalRect.x = 0;
		finalRect.w = uiRect.w;
	}

	if (tMap->height < uiRect.h) {
		finalRect.y = mapOffsetY;
		finalRect.h = tMap->height;
	}
	else {
		finalRect.y = 0;
		finalRect.h = uiRect.h;
	}

	SDL_RenderCopy(renderer, tMap->texture, &mapRect, &finalRect);
	SDL_Point realPoint = CoordToPoint(mouseX, mouseY);
	realPoint.x -= cameraX;
	realPoint.y -= cameraY;
	Sint16 x[4], y[4];

	//Bounding Rectangle
	/*x[0] = realPoint.x; y[0] = realPoint.y;
	x[1] = realPoint.x + tileWidth; y[1] = realPoint.y;
	x[3] = realPoint.x; y[2] = realPoint.y + tileHeight;
	x[2] = realPoint.x + tileWidth; y[3] = realPoint.y + tileHeight;*/
	//Tile rectangle
	x[0] = realPoint.x + tileWidth/2; y[0] = realPoint.y;
	x[1] = realPoint.x + tileWidth; y[1] = realPoint.y + tileHeight/2;
	x[2] = realPoint.x + tileWidth / 2; y[2] = realPoint.y + tileHeight;
	x[3] = realPoint.x; y[3] = realPoint.y + tileHeight/2;

	//filledPolygonColor(renderer, x, y, 4, (Uint32)0x80FFFFFF);

	//renderMasksSolid();
	renderObjects();

	//NPCs
	/*for (auto npc : npcs) {
		if (doRectIntersect(npc->getRenderRect(), mapRect)) {
			npc->render();
		}
		else {
			if (npc->GetSprite()) npc->GetSprite()->stop();
		}
	}*/

	if (colosseum) {
		//colosseum->render(-posX, -posY);
	}

	renderMasksTransparent();

	//SDL_SetRenderTarget(renderer, origTarget);

	//SDL_RenderCopy(renderer, final_map, NULL, &renderRect);
	//SDL_DestroyTexture(final_map);

	//Other Users
	/*std::vector<User*> userList = userManager.getUsers();
	for (auto user : userList) {
		if (doRectIntersect(user->getRenderRect(), mapRect)) {
			user->render();
		}
	}*/

	//player->render();

	//Battle Results
	if (battleResult && !battle) {
		battleResult->render();
		if (battleResult->getElapsedTime() > 5) {
			deleteBattleResult();
		}
	}
}

void GameMap::renderObjects() {
	std::vector<GameObj*> objects;
	//npc, user, player, mapObject

	for (auto obj : mapObjects) addObjectByCoord(objects, obj);
	for (auto npc : npcs) addObjectByCoord(objects, npc);
	std::vector<User*> userList = userManager.getUsers();
	for (auto user : userList) addObjectByCoord(objects, user);
	addObjectByCoord(objects, player);

	for (auto object : objects) {
		if (doRectIntersect(object->GetRenderRect(), mapRect)) {
			object->render();
		}
		else {
			if (object->GetSprite()) object->GetSprite()->stop();
		}
	}
}

void GameMap::addObjectByCoord(std::vector<GameObj*> &vSort, GameObj* object) {
	if (!object) return;
	std::vector<GameObj*>::iterator itr;
	itr = vSort.begin();

	SDL_Point pointObject = object->GetPosition();
	while (itr != vSort.end()) {
		GameObj *compare = *itr;

		if (compare) {
			SDL_Point pointCompare = compare->GetPosition();
			if (pointObject.y <= pointCompare.y) break;
			itr++;
		}
	}
	vSort.insert(itr, object);
}


//Renders map object masks when approptiate - creates the 'see-through' effect when a character is behind an object
void GameMap::renderMasksSolid() {
	for (int i = 0; i < mapObjects.size(); i++) {
		Sprite* nextSprite = mapObjects.at(i)->GetSprite();
		if (nextSprite->subimages.size() == 0) continue;

		if (doRectIntersect(nextSprite->getRenderRect(), mapRect)) {
			//only render visible objects
			nextSprite->resume();
			nextSprite->render(mapOffsetX - cameraX, mapOffsetY - cameraY);
		}
		else nextSprite->stop();
	}
}

void GameMap::renderMasksTransparent() {
	std::vector<GameObj*> objects;
	for (auto obj : mapObjects)  objects.push_back(obj);
	for (auto npc : npcs) objects.push_back(npc);
	std::vector<User*> userList = userManager.getUsers();
	for (auto user : userList) objects.push_back(user);

	for (auto object : objects) {
		if (object->GetSprite()->subimages.size() == 0) continue;
		if (doRectIntersect(object->GetRenderRect(), mapRect)) {

			bool draw = false;
			SDL_Rect objRect = object->GetRenderRect();
			SDL_Rect playerRect = player->GetRenderRect();
			if (!doRectIntersect(objRect, playerRect)) continue;

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

			//Find the pixels that intersect and draw only those pixels with alpha blending on top
			for (int y = 0; y < playerRect.h; y++) {
				for (int x = 0; x < playerRect.w; x++) {
					SDL_Point getPixel = { x, y };
					Asset currentTexture = player->GetSprite()->getCurrentTexture();
					if (currentTexture && currentTexture->loaded) {
						SDL_Color playerPixel = currentTexture->getPixelColor(getPixel);

						if (playerPixel.a > 0) {
							//the pixel becomes see-through, so its alpha is reduced
							double alpha = playerPixel.a * 0.25;
							playerPixel.a = alpha;
							SDL_SetRenderDrawColor(renderer, playerPixel.r, playerPixel.g, playerPixel.b, playerPixel.a);
							SDL_Point destPoint = { playerRect.x + getPixel.x - cameraX, playerRect.y + getPixel.y - cameraY };
							SDL_RenderDrawPoint(renderer, destPoint.x, destPoint.y);
						}
					}
				}
			}

			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
		}
	}
}


void GameMap::step() {
	player->step();

	userManager.stepAllUsers();

	//Can we change the checkPortal to a getJumping check?
	if (!changingMap && (checkPortal || player->getWalking())) {
		checkForMapChange();
		checkPortal = false;
	}
}


void GameMap::checkForMapChange() {
	if (player->GetTeam() && !player->IsTeamLeader()) return;
	int portalId;
	SDL_Point coord = player->GetCoord();
	if (isCoordAPortal(coord, &portalId)) {
		pAction* portalPacket = new pAction(player->AccountId, player->GetID(), portalId, coord.x, coord.y, amLeave);
		gClient.addPacket(portalPacket);
		changingMap = true;
	}
}


void GameMap::setMapPos(int x, int y) {
	//x -= 20;
	//y -= 9;
	//x and y are relative to the map and are centered around point
	cameraX = (x - (uiRect.w / 2));
	int xOffset = tMap->width - uiRect.w;
	if (cameraX > xOffset) cameraX = xOffset;
	if (cameraX < 0) cameraX = 0;

	cameraY = (y - (uiRect.h / 2));
	int yOffset = tMap->height - uiRect.h;
	if (cameraY > yOffset) cameraY = yOffset;
	if (cameraY < 0) cameraY = 0;
}


void GameMap::setMouseCoordinates(int x, int y) {
	int mx = x + cameraX;
	int my = y + cameraY;
	PointToCoord(mx, my, mouseX, mouseY);
}


void GameMap::PointToCoord(int x, int y, int &coord_x, int &coord_y) {
	int origin_x = (mapWidth * 64 / 2);
	int origin_y = 16;

	int adjust_y = mapHeight % 2 == 0 ? 1 : 0;
	int offset_x = origin_x - (tMap->width / 2);
	int offset_y = origin_y + (32 * (mapHeight - adjust_y) / 2) - (tMap->height / 2);

	int iWorldX = (offset_x + x) - origin_x;
	int iWorldY = (offset_y + y) - origin_y;

	double dWorldX = (double)iWorldX;
	double dWorldY = (double)iWorldY;
	double dCellWidth = (double)64;
	double dCellHeight = (double)32;

	double dTemp0 = (1.0*dWorldX) / (1.0*dCellWidth) + (1.0*dWorldY) / (1.0*dCellHeight);
	double dTemp1 = (1.0*dWorldY) / (1.0*dCellHeight) - (1.0*dWorldX) / (1.0*dCellWidth);
	coord_x = doubleToInt(dTemp0);
	coord_y = doubleToInt(dTemp1);
}


int GameMap::doubleToInt(double dValue) {
	if ((int)(dValue + 0.5) >(int)dValue)
		return int(dValue) + 1;
	else
		return int(dValue);
}


SDL_Point GameMap::CoordToPoint(SDL_Point point) {
	return CoordToPoint(point.x, point.y);
}

SDL_Point GameMap::CoordToPoint(int x, int y) {
	if (!tMap) return SDL_Point{ 0, 0 };
	int offset_x = (tMap->width / 2);
	int offset_y = (32 * mapHeight / 2) - (tMap->height / 2);
	int adjust_y = mapHeight % 2 == 0 ? 0 : 1;

	SDL_Point newPoint;
	newPoint.x = ((x - y - 1) * tileWidth / 2) + offset_x;
	newPoint.y = ((x + y - adjust_y) * tileHeight / 2) - offset_y;
	return newPoint;
}

SDL_Point GameMap::CenterOfCoord(SDL_Point coord) {
	return CenterOfCoord(coord.x, coord.y);
}

SDL_Point GameMap::CenterOfCoord(int x, int y) {
	SDL_Point centerPoint = CoordToPoint(x, y);
	centerPoint.x += tileWidth / 2;
	centerPoint.y += tileHeight / 2;
	return centerPoint;
}


bool GameMap::isPointInTrigon(int x, int y, SDL_Point a, SDL_Point b, SDL_Point c) {
	bool b1, b2, b3;
	b1 = ((x - b.x) * (a.y - b.y) - (a.x - b.x) * (y - b.y)) < 0.0f;
	b2 = ((x - c.x) * (b.y - c.y) - (b.x - c.x) * (y - c.y)) < 0.0f;
	b3 = ((x - a.x) * (c.y - a.y) - (c.x - a.x) * (y - a.y)) < 0.0f;
	return ((b1 == b2) && (b2 == b3));
}


bool GameMap::doRectIntersect(SDL_Rect a, SDL_Rect b) {
	if (a.x <= b.x + b.w && a.x + a.w >= b.x &&
		a.y <= b.y + b.h && a.y + a.h >= b.y) return true;
	return false;
}

bool GameMap::isPointAPortal(SDL_Point aPoint, int &portalId) {
	int coord_x, coord_y;
	PointToCoord(aPoint.x, aPoint.y, coord_x, coord_y);
	
	portalId = -1;
	for (int i = 0; i < portals.size(); i++) {
		if (portals.at(i).x == coord_x && portals.at(i).y == coord_y) {
			portalId = portals.at(i).id;
			return true;
		}
	}
	return false;
}

bool GameMap::isCoordAPortal(SDL_Point aCoord) {
	for (int i = 0; i < portals.size(); i++) {
		if (portals.at(i).x == aCoord.x && portals.at(i).y == aCoord.y) {
			return true;
		}
	}
	return false;
}

bool GameMap::isCoordAPortal(SDL_Point aCoord, int* portalId) {
	*portalId = -1;
	for (int i = 0; i < portals.size(); i++) {
		if (portals.at(i).x == aCoord.x && portals.at(i).y == aCoord.y) {
			*portalId = portals.at(i).id;
			return true;
		}
	}
	return false;
}

int GameMap::getMapId() {
	return id;
}


int GameMap::getMapDoc() {
	return doc;
}


void GameMap::setMapUiRect(int xOffset, int yOffset, int width, int height) {
	uiRect = { xOffset, yOffset, width, height };
	renderRect.w = width;
	renderRect.h = height;
}

void GameMap::setMapUiRect(SDL_Rect rUI) {
	uiRect = rUI;
	renderRect.w = rUI.w;
	renderRect.h = rUI.h;
}

bool GameMap::getJumpMode() {
	return jumpMode;
}

void GameMap::toggleJumpMode() {
	jumpMode = !jumpMode;
}


std::vector<SDL_Point> GameMap::getPath(SDL_Point from, SDL_Point to) {
	std::vector<SDL_Point> path;
	if (from.x == to.x && from.y == to.y) return path;

	//Walkable coords may have a straight path
	if (isCoordWalkable(to)) {

		//Move straight in odd direction 1/3/5/7
		if (from.x == to.x || from.y == to.y) {
			//Path is straight, verify no obstructions

			//Only Y axis changed
			if (from.x == to.x) {
				if (from.y < to.y) {
					for (int i = from.y + 1; i < to.y; i++) {
						SDL_Point aCoord = { from.x, i };
						if (!isCoordWalkable(aCoord)) {
							path.clear();
							break;
						}
						path.push_back(aCoord);
					}
					if (path.size() > 0 || from.y == (to.y - 1)) path.push_back(to);
				}

				if (from.y > to.y) {
					for (int i = from.y - 1; i > to.y; i--) {
						SDL_Point aCoord = { from.x, i };
						if (!isCoordWalkable(aCoord)) {
							path.clear();
							break;
						}
						path.push_back(aCoord);
					}
					if (path.size() > 0 || from.y == (to.y + 1)) path.push_back(to);
				}
			}

			//Only Y axis changed
			if (from.y == to.y) {
				if (from.x < to.x) {
					for (int i = from.x + 1; i < to.x; i++) {
						SDL_Point aCoord = { i, from.y };
						if (!isCoordWalkable(aCoord)) {
							path.clear();
							break;
						}
						path.push_back(aCoord);
					}
					if (path.size() > 0 || from.x == (to.x-1)) path.push_back(to);
				}

				if (from.x > to.x) {
					for (int i = from.x - 1; i > to.x; i--) {
						SDL_Point aCoord = { i, from.y };
						if (!isCoordWalkable(aCoord)) {
							path.clear();
							break;
						}
						path.push_back(aCoord);
					}
					if (path.size() > 0 || from.x == (to.x + 1)) path.push_back(to);
				}
			}

			if (path.size() == 0) return generatePath(from, to); //There was an obstruction, generate a path
			else return path;
		}

		//Move straight in even direction 0/2/4/6
		double changeX, changeY;
		changeX = abs(from.x - to.x);
		changeY = abs(from.y - to.y);
		if (changeX == changeY) {
			std::cout << "Move straight diagonal.\n";
			SDL_Point aCoord = from;
			for (int i = 0; i < (int)std::round(changeX); i++) {
				if (aCoord.x < to.x) aCoord.x++;
				if (aCoord.x > to.x) aCoord.x--;
				if (aCoord.y < to.y) aCoord.y++;
				if (aCoord.y > to.y) aCoord.y--;
				if (!isCoordWalkable(aCoord)) {
					path.clear();
					break;
				}
				path.push_back(aCoord);
			}

			if (path.size() == 0) return generatePath(from, to); //There was an obstruction, generate a path
			else return path;
		}

		return generatePath(from, to);
	}

	//optimization for unwalkable?
	//Need to develop "closest path to point" logic
	return generatePath(from, to);

	//return path;
}


std::vector<SDL_Point> GameMap::generatePath(SDL_Point from, SDL_Point to) {
	std::vector<SDL_Point> path;

	std::vector<PathTile*> openList, closedList;
	PathTile *dest = nullptr;

	std::cout << "Generating a path.\n";
	int startTime = SDL_GetTicks();
	PathTile *start = new PathTile;
	start->coord = from;
	calcTileScores(start, to);
	openList.push_back(start);
	do {
		PathTile* currentTile = openList.at(0);
		openList.erase(openList.begin());
		
		closedList.push_back(currentTile);
		//std::cout << "Tile Added to Closed List: " << "[" << currentTile->coord.x << "," << currentTile->coord.y << "] " << "D: " << currentTile->distanceScore << ", M:" << currentTile->moveCost << ", W:" << currentTile->weightedScore << ", Iter:" << currentTile->depth << " \n";
		dest = getTileFromList(to, &closedList);
		if (dest != nullptr) break;

		std::vector<PathTile*> adjacentTiles;
		adjacentTiles = getAdjacentTiles(*currentTile, to); //This should first check if dest is 1 of the 8, and only return dest if so

		for (auto &adjTile : adjacentTiles) {
			if (getTileFromList(adjTile->coord, &closedList) != nullptr) {
				delete adjTile;
				continue;
			}
			adjTile->parent = currentTile;
			calcTileScores(adjTile, to);

			if (adjTile->depth > 15) {
				delete adjTile;
				continue;
			}

			PathTile* tileFromOpenList = getTileFromList(adjTile->coord, &openList);
			if (tileFromOpenList == nullptr) {
				addTileToOpenList(adjTile, &openList);
			}
			else {
				//calc values for adjTile
				if (adjTile->weightedScore < tileFromOpenList->weightedScore) {
					for (int i = 0; i < openList.size(); i++) {
						if (openList.at(i) == tileFromOpenList) {
							openList.erase(openList.begin() + i);
							break;
						}
					}
					addTileToOpenList(adjTile, &openList);
				}
			}
		}
	} while (openList.size() > 0);

	//Destination not found, get best path instead
	if (!dest) {
		PathTile *bestTile = nullptr;
		for (auto closedTile : closedList) {
			if (!bestTile) {
				bestTile = closedTile;
				continue;
			}

			bool newBest = false;
			if (closedTile->distanceScore < bestTile->distanceScore) newBest = true;
			if (closedTile->distanceScore == bestTile->distanceScore && closedTile->weightedScore < bestTile->weightedScore) newBest = true;

			if (newBest) {
				bestTile = closedTile;
				std::cout << "New Best Tile: " << "[" << closedTile->coord.x << "," << closedTile->coord.y << "] " << "D: " << closedTile->distanceScore << ", M:" << closedTile->moveCost << ", W:" << closedTile->weightedScore << ", Iter:" << closedTile->depth << " \n";
				continue;
			}
		}
		dest = bestTile;
	}

	//generate path in reverse from dest
	if (dest) {
		PathTile* nextPathTile = dest;
		while (nextPathTile) {
			if (from.x != nextPathTile->coord.x || from.y != nextPathTile->coord.y) path.insert(path.begin(), nextPathTile->coord);
			nextPathTile = nextPathTile->parent;
		}
		std::cout << "Path found!\n";
	}

	int endTime = SDL_GetTicks() - startTime;
	std::cout << "Path generation ended. Ticks passed: " << endTime << "\n";

	return path;
}


PathTile* GameMap::getTileFromList(SDL_Point coord, std::vector<PathTile*> *tileList) {
	for (auto &aTile : *tileList) {
		if (aTile->coord.x == coord.x && aTile->coord.y == coord.y) return aTile;
	}

	return nullptr;
}


std::vector<PathTile*> GameMap::getAdjacentTiles(PathTile sourceTile, SDL_Point destCoord) {
	std::vector<PathTile*> adjacentTiles;

	double changeX, changeY;
	changeX = abs(sourceTile.coord.x - destCoord.x);
	changeY = abs(sourceTile.coord.y - destCoord.y);

	//Check if destination is adjacent
	if (((int)std::round(changeX) == 1 || (int)std::round(changeX) == 0) && ((int)std::round(changeY) == 1 || (int)std::round(changeY) == 0)) {
		if (isCoordWalkable(destCoord)) {
			PathTile* dest = new PathTile;
			dest->coord = destCoord;
			adjacentTiles.push_back(dest);
		}
		return adjacentTiles;
	}

	//Add all adjacent tiles
	for (int i = 0; i < 8; i++) 
			addAdjacentTile(&adjacentTiles, getAdjCoordByDirection(sourceTile.coord, i));

	return adjacentTiles;
}


SDL_Point GameMap::getAdjCoordByDirection(SDL_Point sourceCoord, int direction) {
	SDL_Point adjCoord = sourceCoord;
	switch (direction) {
	case 7:
		adjCoord.x += 1;
		adjCoord.y += 1;
		break;
	case 0:
		adjCoord.y += 1;
		break;
	case 1:
		adjCoord.x -= 1;
		adjCoord.y += 1;
		break;
	case 2:
		adjCoord.x -= 1;
		break;
	case 3:
		adjCoord.x -= 1;
		adjCoord.y -= 1;
		break;
	case 4:
		adjCoord.y -= 1;
		break;
	case 5:
		adjCoord.x += 1;
		adjCoord.y -= 1;
		break;
	case 6:
		adjCoord.x += 1;
		break;
	}

	return adjCoord;
}


void GameMap::addAdjacentTile(std::vector<PathTile*> *adjacentTiles, SDL_Point coord) {
	if (!isCoordWalkable(coord)) return;

	PathTile* newTile = new PathTile;
	newTile->coord = coord;
	adjacentTiles->push_back(newTile);
}


void GameMap::calcTileScores(PathTile* adjTile, SDL_Point to) {
	SDL_Point sourcePoint = CenterOfCoord(adjTile->coord.x, adjTile->coord.y);
	SDL_Point destPoint = CenterOfCoord(to.x, to.y);

	int nDx = abs(to.x - adjTile->coord.x);
	int nDy = abs(to.y - adjTile->coord.y);
	if (nDx > nDy) adjTile->distanceScore = 10 * nDx + 6 * nDy;
	else adjTile->distanceScore = 10 * nDy + 6 * nDx;
	
	int parentMoveCost = 0;
	if (adjTile->parent) {
		parentMoveCost = adjTile->parent->moveCost;
		adjTile->depth = adjTile->parent->depth;
	}
	adjTile->depth++;

	if (adjTile->coord.x == to.x || adjTile->coord.y == to.y) adjTile->moveCost = parentMoveCost + 10;
	else adjTile->moveCost = parentMoveCost + 14;

	adjTile->weightedScore = adjTile->distanceScore + adjTile->moveCost;
}


void GameMap::addTileToOpenList(PathTile* adjTile, std::vector<PathTile*> *openList) {
	for (int i = 0; i < openList->size(); i++) {
		PathTile *nextTile = openList->at(i);
		if (nextTile->weightedScore > adjTile->weightedScore) {
			
			openList->insert(openList->begin() + i, adjTile);
			return;
		}
	}

	openList->push_back(adjTile);
}


bool GameMap::isCoordWalkable(SDL_Point coord) {
	int pos = (coord.y*mapWidth) + coord.x;
	int walkable = 48 - (int)mapCoordinates[pos];
	//std::cout << "Coord [" << coord.x << "," << coord.y << "] is";
	if (walkable == 0) { /*std::cout << " walkable\n";*/ return true; }
	//std::cout << " not walkable\n";  
	return false;
}


int GameMap::getDirectionToCoord(SDL_Point fromCoord, SDL_Point toCoord) {
	SDL_Point fromPos = CenterOfCoord(fromCoord.x, fromCoord.y);
	SDL_Point toPos = CenterOfCoord(toCoord.x, toCoord.y);
	return getDirection(fromPos, toPos);
}


void GameMap::createDialogue(pNpcDialogue* packet) {
	if (dialogue) delete dialogue;

	std::string name = "";
	if (dialogueNpc) name = dialogueNpc->GetName();

	dialogue = new Dialogue(mainForm, packet, name, (map->uiRect.w / 2), 10);
	dialogue->setWindowOffset(SDL_Point{ uiRect.x, uiRect.y });
}

void GameMap::addBattleResult(BattleResult* br) {
	deleteBattleResult();
	battleResult = br;
}

void GameMap::deleteBattleResult() {
	if (!battleResult) return;
	delete battleResult;
	battleResult = nullptr;
}

bool GameMap::doesBattleResultExist() {
	if (battleResult) return true;
	return false;
}