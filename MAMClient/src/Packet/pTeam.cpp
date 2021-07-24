#include "stdafx.h"
#include "pTeam.h"

#include "CustomEvents.h"

#include "UserManager.h"
#include "Player.h"
#include "Team.h"

pTeam::pTeam(char* buf, char* encBuf) {
	description = "Team (Server)";
	type = ptTeam;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &action);
	getInt(4, &sourceId);
	getInt(8, &targetId);
}

pTeam::pTeam(int act, int sId, int tId) {
	description = "Team (Client)";
	type = ptTeam;
	initBuffer(16);

	action = act;
	sourceId = sId;
	targetId = tId;

	addInt(0, action);
	addInt(4, sourceId);
	addInt(8, targetId);
}


pTeam::~pTeam() {
	//
}


void pTeam::process() {
	User* sourceUser = userManager.getUserById(sourceId);
	User* targetUser = userManager.getUserById(targetId);

	switch (action) {
	case TEAM_ACT_CREATE:
		if (sourceUser) sourceUser->CreateTeam();
		break;
	case TEAM_ACT_DISBAND:
		if (sourceUser) sourceUser->LeaveTeam();
		break;
	case TEAM_ACT_JOIN:
		if (sourceUser && targetUser) {
			sourceUser->JoinTeam(targetUser->GetTeam());
		}
		break;
	case TEAM_ACT_QUIT:
		if (sourceUser) sourceUser->LeaveTeam();
		break;
	}

	if (sourceUser == player || targetUser == player) {
		SDL_Event e;
		SDL_zero(e);
		e.type = CUSTOMEVENT_PLAYER;
		e.user.code = PLAYER_TEAM;
		SDL_PushEvent(&e);
	}
}


void pTeam::debugPrint() {
	Packet::debugPrint();

	std::cout << "Action:" << action << " Source ID: " << sourceId << " Target ID: " << targetId << std::endl;

	std::cout << std::endl;
}