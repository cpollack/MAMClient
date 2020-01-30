#pragma once

#include "GameLibrary.h"
#include "Texture.h"
#include "Sprite.h"
#include "MapFile.h"
#include "pMapInfo.h"
#include "Dialogue.h"
#include "Colosseum.h"

using Asset = std::shared_ptr<Texture>;

class BattleResult;
class NPC;

typedef enum TileZone {
	CENTER,
	TOPLEFT,
	TOPRIGHT,
	BOTTOMLEFT,
	BOTTOMRIGHT
}TileZone;

struct PathTile {
	SDL_Point coord;
	PathTile *parent = nullptr;
	int moveCost = 0;
	int distanceScore = 0;
	int weightedScore = 0;
	int depth = 0;
};

class GameMap {
public:
	GameMap(pMapInfo* packet);
	~GameMap();

	void ReloadAssets();
	void render();
	bool handleEvent(SDL_Event& e);
	void handleCustomEvent(SDL_Event& e);
	void step();

private: // Events
	void OnClick(SDL_Event& e);
	void OnMouseMove(SDL_Event& e);

private:
	void loadMapFile(int mapDoc);
	void renderMasks();

public:
	SDL_Renderer* renderer;
	SDL_Rect renderRect;
	
	SDL_Rect uiRect = { 0, 0, 740, 410 };
	int uiX, uiY;

	SDL_Texture* tileTexture;
	SDL_Rect tileRect;
	const int tileWidth = 64;
	const int tileHeight = 32;
	short tileZone[64][32] = { {0} };

	Asset tMap;
	SDL_Rect mapRect;
	int mapWidth, mapHeight;
	int cameraX, cameraY;
	int mouseX, mouseY;

	char *mapCoordinates;
	std::vector<MapPortal> portals;
	std::vector<std::vector<Asset>> mapAssets;
	std::vector<std::string> masks;
	std::vector<Sprite*> objects;
	std::vector<NPC*> npcs;

	NPC* focusedNPC = nullptr;
	
	void checkForMapChange();
	bool IsChangingMap() { return changingMap; }

	void GameMap::addNpc(pNpcInfo* packet);
	void createDialogue(pNpcDialogue* packet);
	void GameMap::addColosseum(pColosseum* packet);

	void GameMap::setMapPos(int x, int y);
	void GameMap::setMouseCoordinates(int x, int y);
	void GameMap::PointToCoord(int x, int y, int &coord_x, int &coord_y);
	SDL_Point GameMap::CoordToPoint(SDL_Point point);
	SDL_Point GameMap::CoordToPoint(int x, int y);
	SDL_Point GameMap::CenterOfCoord(SDL_Point coord);
	SDL_Point GameMap::CenterOfCoord(int x, int y);
	bool GameMap::isPointInTrigon(int x, int y, SDL_Point a, SDL_Point b, SDL_Point c);
	bool GameMap::doRectIntersect(SDL_Rect a, SDL_Rect b);
	int GameMap::doubleToInt(double dValue);

	bool isCoordAPortal(SDL_Point aCoord);
	bool GameMap::isCoordAPortal(SDL_Point aCoord, int* portalId);
	bool GameMap::isPointAPortal(SDL_Point aPoint, int &portalId);

	std::vector<SDL_Point> GameMap::getPath(SDL_Point from, SDL_Point to);
	std::vector<SDL_Point> GameMap::generatePath(SDL_Point from, SDL_Point to);
	PathTile* GameMap::getTileFromList(SDL_Point coord, std::vector<PathTile*> *tileList);
	std::vector<PathTile*> GameMap::getAdjacentTiles(PathTile sourceTile, SDL_Point destCoord);
	void GameMap::addAdjacentTile(std::vector<PathTile*> *adjacentTiles, SDL_Point coord);
	SDL_Point GameMap::getAdjCoordByDirection(SDL_Point sourceCoord, int direction);
	void GameMap::calcTileScores(PathTile* adjTile, SDL_Point to);
	void GameMap::addTileToOpenList(PathTile* adjTile, std::vector<PathTile*> *openList);
	bool GameMap::isCoordWalkable(SDL_Point coord);

	int GameMap::getDirectionToCoord(SDL_Point fromCoord, SDL_Point toCoord);

	int GameMap::getMapId();
	int GameMap::getMapDoc();

	void setMapUiRect(int xOffset, int yOffset, int width, int height);
	void setMapUiRect(SDL_Rect rUI);

	bool getJumpMode();
	void toggleJumpMode();

private:
	std::string name;
	int id;
	int doc, type;
	int weather, bgMusic, bgMusic_show, fightMusic;

	Colosseum* colosseum = nullptr;

	bool mapClicked_left=false, mapClicked_right=false;
	bool jumpMode = false, checkPortal = false, changingMap = false;

	NPC* dialogueNpc = nullptr;
	int shopId = 0;
	Dialogue* dialogue = nullptr;

public: 
	void addBattleResult(BattleResult* br);
	void deleteBattleResult();
	bool doesBattleResultExist();
};

extern GameMap *map;