#include "stdafx.h"
#include "Client.h"
#include "pAction.h"

#include "Player.h"
#include "User.h"
#include "UserManager.h"


pAction::pAction(char *buf, char* encBuf) {
	description = "Action (Server)";
	type = ptAction;
	initBuffer(24);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getWord(4, &posX);
	getWord(6, &posY);
	getInt(8, &direction);
	getInt(12, &mode);

	int verif = 0;
	getInt(16, &verif);
}


pAction::pAction(int accountId, int uId, int dir, int pX, int pY, int mode) {
	description = "Action (Client)";
	type = ptAction;
	initBuffer(24);

	userId = uId;
	direction = dir;
	posX = pX;
	posY = pY;
	this->mode = mode;

	addDWord(0, uId);
	addWord(4, pX);
	addWord(6, pY);
	addDWord(8, dir);
	addDWord(12, mode);

	DWORD verif = 0;
	verif = (accountId + userId) ^ timeGetTime();
	addDWord(16, verif);
}


pAction::~pAction() {
	//
}


void pAction::process() {
	//The position packet is used for many things beyond setting a player position
	//It is also used to set states and animations for players
	//userId - id of affected user
	//mode - the action taken
	//x/y always refers to coords
	//direction - is replaced with a value according to mode
	User *user = nullptr;

	switch (mode) {
	case (amNone):
			//
			break;
	case (amJump):
		user = userManager.getUserById(userId);
		if (user) {
			user->jumpTo(SDL_Point{ posX, posY });
		}
		break;
	case (amLeave):
		user = userManager.getUserById(userId);
		if (user) {
			user->setLeaving(true);
		}
	case (amFly):
		user = userManager.getUserById(userId);
		if (user) user->TakeOff();
		break;
	case (amLand):
		user = userManager.getUserById(userId);
		if (user) user->Land();
		break;
	case (amEmote):
		user = userManager.getUserById(userId);
		if (user) {
			int emote = direction;
			user->SetEmotion(emote);
			user->loadSprite();
		}
	}

}


void pAction::debugPrint() {
	Packet::debugPrint();

	std::string modeText;
	switch (mode) {
	case amNone: modeText = "None"; break;
	case amJump: modeText = "Jump"; break;
	case amDisguise: modeText = "Disguise"; break;
	case amLeave: modeText = "LeaveMap"; break;
	case amFly: modeText = "FlyCloud"; break;
	case amLand: modeText = "LandCloud"; break;
	default: modeText = "Unknown";
	}
	std::cout << "Mode: " << mode << " - " << modeText << std::endl;
	std::cout << "User Id: " << userId << " Position: " << posX << "," << posY << " Dir: " << direction << std::endl;

	std::cout << std::endl;
}