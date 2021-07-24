#include "stdafx.h"
#include "pUserLevelUp.h"

#include "CustomEvents.h"
#include "BattleResult.h"
#include "Player.h"

pUserLevelUp::pUserLevelUp(char* buf, char* encBuf) {
	description = "User Levelup (Server)";
	type = ptUserLevelUp;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getDWord(0, &userId);
	getInt(4, &experience);
	getWord(8, &level);
	getWord(10, &attributePoints);
}


pUserLevelUp::~pUserLevelUp() {
	//
}


void pUserLevelUp::process() {
	if (player->GetID() != userId) return;

	player->SetLevel(level);
	player->SetExperience(experience);
	player->SetUnusedPoints(attributePoints);

	SDL_Event e;
	SDL_zero(e);
	e.type = CUSTOMEVENT_PLAYER;
	e.user.code = PLAYER_LEVEL;
	SDL_PushEvent(&e);

	PlayerLeveled = true;
}


void pUserLevelUp::debugPrint() {
	Packet::debugPrint();

	std::cout << "User Id:" << userId << " Level:" << level << " Exp" << experience << " Attribute Points:" << attributePoints << std::endl;

	std::cout << std::endl;
}