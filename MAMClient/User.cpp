#include "stdafx.h"
#include "User.h"
#include "GameMap.h"

#include "pUserInfo.h"
#include "MainWindow.h"

User::User(pUserInfo *packet):Entity(mainForm->renderer, packet->userId, packet->name, packet->look) {
	NickName = packet->nickName;
	Spouse = packet->spouse;
	setCoord(SDL_Point{ packet->x, packet->y });
	setDirection(packet->direction);
	setAnimation(StandBy); //TEMP - Does the packet define the animation?

	Level = packet->level;
	Rank = packet->rank;
	Reborns = packet->reborns;
	RankType = packet->rankType; //Needed?
	MasterRank = packet->masterType;
	Alignment = packet->alignment;
	FullRank = (MasterRank * 10000) + (Rank * 1000) + (Reborns * 10) + Alignment;
	
	pkEnabled = packet->pkEnabled;
	SyndicateId = packet->syndicateId;
	SubGroudId = packet->subGroudId;
	SyndicateRank = packet->syndicateRank;
	Guild = packet->guild;
	SubGroup = packet->subGroup;
	GuildTitle = packet->guildTitle;
	
	memcpy(colorSets, packet->colorSets, 25);

	switch (packet->emotion) {
	case uieNone:
		Animation = StandBy;
		break;
	case uieWave:
		Animation = SayHello;
		break;
	case uieFaint:
		Animation = Faint;
		break;
	case uieSit:
		Animation = SitDown;
		break;
	case uieKneel:
		Animation = Genuflect;
		break;
	case uieHappy:
		Animation = Laugh;
		break;
	case uieBow:
		Animation = Politeness;
		break;
	default:
		Animation = StandBy;
	}
}

User::User(SDL_Renderer* r, int id, std::string name, int look):Entity(r, id, name, look) {
	setAnimation(StandBy); //TEMP - Does the packet define the animation?
}

User::~User() {
	if (effect) delete effect;
	if (aura) delete aura;
}

void User::render() {
	if (!map) return;
	Entity::render();

	if (effect) effect->render(RenderPos.x, RenderPos.y);
	//if (aura) aura->render(effectsX, effectsY);

	renderNameplate();
}


void User::renderNameplate() {
	SDL_Color nameColor = { 255, 255, 0, 255 };
	SDL_Color guildColor = { 255, 255, 0, 255 };
	SDL_Color shadow = { 32, 32, 32, 192 };

	SDL_Surface *nameSurface = TTF_RenderText_Blended(gui->chatFont, Name.c_str(), nameColor);
	SDL_Surface *shadowSurface = TTF_RenderText_Blended(gui->chatShadowFont, Name.c_str(), shadow);
	SDL_Texture *nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
	SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);

	int w = nameSurface->w;
	int h = nameSurface->h;
	SDL_FreeSurface(nameSurface);
	SDL_FreeSurface(shadowSurface);

	if (nameTexture) {
		SDL_Rect srcRect = { 0, 0, w, h };
		SDL_Rect destRect = { RenderPos.x - (w / 2), RenderPos.y - 82 - h, w, h };

		SDL_SetTextureBlendMode(nameTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(shadowTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

		SDL_RenderCopy(renderer, shadowTexture, &srcRect, &destRect);
		destRect.x -= 1;
		destRect.y -= 1;
		SDL_RenderCopy(renderer, nameTexture, &srcRect, &destRect);
		SDL_RenderCopy(renderer, nameTexture, &srcRect, &destRect);

		SDL_DestroyTexture(nameTexture);
		SDL_DestroyTexture(shadowTexture);
	}
}


void User::step() {
	if (jumping) {
		if (effect && effect->finished()) {
			delete effect;
			effect = nullptr;
			jumping = false;
			setAnimation(StandBy);
			loadSprite();
		}
	}

	if (walking) {
		if (atDestCoord()) {
			if (!path.size()) {
				walking = false;
				setAnimation(StandBy);
				loadSprite();
			}
			else {
				getNextDestCoord();
				setDirection(map->getDirectionToCoord(Coord, DestCoord));
				loadSprite();
			}
		}
		else {
			takeNextStep();
		}
	}
}

void User::handleEvent(SDL_Event& e) {
	//standard event handling and call mousemove subfunc
	//check for mouse over
	//intersecting, check if valid pixel
	//callback function for displaying info?
}

void User::jumpTo(SDL_Point coord) {
	if (walking) {
		walking = false;
		path.clear();
		DestCoord = { 0,0 };
	}

	jumping = true;

	setDirectionToCoord(coord);
	setCoord(coord);
	setAnimation(Genuflect);
	loadSprite();
	loadEffect("FlashDown");
	lastMoveTick = timeGetTime();
}

void User::walkTo(SDL_Point coord) {
	std::vector<SDL_Point> newPath = map->getPath(Coord, coord);

	if (jumping) {
		jumping = false;
		//Clear Flashdown?
	}

	path.clear();
	if (newPath.size() == 0) return;

	path = newPath;
	//When User is already walking, do not overwrite the current destination coord
	if (walking && atDestCoord() || !walking) getNextDestCoord();
	walking = true;

	setAnimation(Walk);
	setDirectionToCoord(DestCoord);
	loadSprite();
}

bool User::atDestCoord() {
	if (Coord.x == DestCoord.x && Coord.y == DestCoord.y) return true;
	return false;
}

void User::getNextDestCoord() {
	if (!path.size()) return;
	DestCoord = path[0];
	path.erase(path.begin());
	lastMoveTick = timeGetTime();

	//If the next Coord is a map portal, end path traversal on portal
	if (map->isCoordAPortal(DestCoord)) {
		path.clear();
	}
}

void User::takeNextStep() {
	int systemTime = timeGetTime();
	int timeElapsed = systemTime - lastMoveTick;

	SDL_Point basePos = map->CenterOfCoord(Coord);
	SDL_Point destPos = map->CenterOfCoord(DestCoord);
	double movePerc = timeElapsed * 1.0 / WALK_SPEED;
	if (movePerc > 1.0) movePerc = 1.0;

	double shiftx = (destPos.x - basePos.x) * movePerc;
	double shifty = (destPos.y - basePos.y) * movePerc;
	Position.x = basePos.x + shiftx;
	Position.y = basePos.y + shifty;
	if (Position.x == destPos.x && Position.y == destPos.y) setCoord(DestCoord);
}

void User::setLeaving(bool leaving) {
	isLeavingMap = leaving;
}

bool User::leaveMap() {
	if (walking || jumping) return false;
	if (!isLeavingMap) return false;
	return true;
}

bool User::getJumping() {
	return jumping;
}

bool User::getWalking() {
	return walking;
}

void User::loadEffect(std::string _effectName) {
	if (effect) delete effect;
	effectName = _effectName;
	INI commonIni("INI\\Common.ini", _effectName);

	//Flash-down Jump Animation
	std::vector<Asset> assets;
	for (int i = 0; i < stoi(commonIni.getEntry("FrameAmount")); i++) {
		std::string nextFrame = "Frame" + std::to_string(i);
		std::string path = commonIni.getEntry(nextFrame);
		path = "data\\" + path.substr(2);
		Asset asset(new Texture(renderer, path, true));
		asset->setBlendMode(SDL_BLENDMODE_ADD);
		assets.push_back(asset);
	}
	effect = new Sprite(renderer, assets, stEffect);
	effect->speed = 2;
	effect->repeatMode = 1;
	effect->start();
}

//Load Aura based on Rank
void User::loadAura() {

}

std::string User::getNickName() {
	return NickName;
}

std::string User::getSpouse() { 
	return Spouse;
}

int User::getLevel() {
	return Level;
}

std::string User::GetRankText() {
	std::string rankText;

	switch (Rank) {
	case 0: //Mortal
		break;
	case 1: 
		rankText = "Basic God";
		break;
	case 2:
		rankText = "Junior "; 
		break;
	case 3:
		rankText = "Senior ";
		break;
	case 4:
		rankText = "Super ";
		break;
	case 5:
		switch (MasterRank) {
		case 1:
			rankText += "Pet Raising Master ";
			break;
		case 2:
			rankText += "Cultivation Master ";
			break;
		case 3:
			rankText += "Virtue Master ";
			break;
		case 4:
			rankText += "Wuxing Master ";
			break;
		case 5:
			rankText += "Kugnfu Master ";
			break;
		case 6:
			rankText += "Thievery Master ";
			break;
		case 7:
			rankText += "Reputation Master ";
			break;
		}
		break;
	}

	if (Alignment > 0) {
		if (Alignment == 1) rankText += "God";
		if (Alignment == 2) rankText += "Devil";
	}

	return rankText;
}

std::string User::getGuild() {
	return Guild;
}

std::string User::getGuildTitle() {
	return GuildTitle;
}