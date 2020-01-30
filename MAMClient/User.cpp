#include "stdafx.h"
#include "User.h"
#include "GameMap.h"

#include "Define.h"
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
	

	for (int i = 0; i < 25; i += 5) {
		ColorShift shift;
		memcpy(&shift, packet->colorSets + i, 5);
		colorShifts.push_back(shift);
	}
	//memcpy(colorSets, packet->colorSets, 25);

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
	if (aura) delete aura;
}

void User::render() {
	if (!map) return;
	Entity::render();

	render_nameplate();
}


void User::step() {
	if (jumping) {
		if (timeGetTime() - lastMoveTick >= JUMP_SPEED) {
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
	addEffect(EFFECT_FLASHDOWN);
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

//Load Aura based on Rank
void User::loadAura() {

}

std::string User::getNickName() {
	return NickName;
}

std::string User::getSpouse() { 
	return Spouse;
}

std::string User::GetRankText() {
	std::string rankText;

	switch (Rank) {
	case 0: //Mortal
		if (Level > 1100) rankText = "Senior Master";
		else if (Level > 900) rankText = "Junior Master";
		else if (Level > 750) rankText = "Senior Knight";
		else if (Level > 500) rankText = "Junior Knight";
		else if (Level > 300) rankText = "Basic Knight";
		else if (Level > 150) rankText = "Senior Warrior";
		else if (Level > 50) rankText = "Junior Warrior";
		else rankText = "Basic Warrior";
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
		rankText = "Master ";
		switch (MasterRank) {
		case 1:
			rankText += "Pet Raising ";
			break;
		case 2:
			rankText += "Cultivation ";
			break;
		case 3:
			rankText += "Virtue ";
			break;
		case 4:
			rankText += "Wuxing ";
			break;
		case 5:
			rankText += "Kungfu ";
			break;
		case 6:
			rankText += "Thievery ";
			break;
		case 7:
			rankText += "Reputation ";
			break;
		}
		break;
	}

	if (Alignment > 0) {
		if (Alignment == 1) rankText += "God";
		if (Alignment == 2) rankText += "Devil";
	}

	if (Reborns > 0) {
		rankText += " [" + std::to_string(Reborns) + "]";
	}

	return rankText;
}

std::string User::getGuild() {
	return Guild;
}

std::string User::getGuildTitle() {
	return GuildTitle;
}