#include "stdafx.h"
#include "Client.h"
#include "pPosition.h"

#include "Player.h"
#include "User.h"
#include "UserManager.h"


pPosition::pPosition(char *buf, char* encBuf) {
	description = "Position (Server)";
	type = ptPosition;
	initBuffer(24);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getWord(4, &posX);
	getWord(6, &posY);
	getInt(8, &value);
	getInt(12, &mode);

	int verif = 0;
	getInt(16, &verif);
}


pPosition::pPosition(int accountId, int uId, int val, int pX, int pY, int mode) {
	description = "Position (Client)";
	type = ptPosition;
	initBuffer(24);

	userId = uId;
	value = val;
	posX = pX;
	posY = pY;
	this->mode = mode;

	addDWord(0, uId);
	addWord(4, pX);
	addWord(6, pY);
	addDWord(8, val);
	addDWord(12, mode);

	DWORD verif = 0;
	verif = (accountId + userId) ^ timeGetTime();
	addDWord(16, verif);
}


pPosition::~pPosition() {
	//
}


void pPosition::process() {
	//The position packet is used for many things beyond setting a player position
	//It is also used to set states and animations for players
	//userId - id of affected user
	//mode - the action taken
	//x/y always refers to coords
	//direction - is replaced with a value according to mode
	User *user = nullptr;

	switch (mode) {
	case (pmNone):
			//
			break;
	case (pmJump):
		user = userManager.getUserById(userId);
		if (user) {
			user->jumpTo(SDL_Point{ posX, posY });
		}
		break;
	case (pmLeave):
		user = userManager.getUserById(userId);
		if (user) {
			user->setLeaving(true);
		}
	}

}


void pPosition::debugPrint() {
	Packet::debugPrint();

	std::string modeText;
	switch (mode) {
	case pmNone: modeText = "None"; break;
	case pmJump: modeText = "Jump"; break;
	case pmDisguise: modeText = "Disguise"; break;
	case pmLeave: modeText = "LeaveMap"; break;
	case pmFly: modeText = "FlyCloud"; break;
	case pmLand: modeText = "LandCloud"; break;
	default: modeText = "Unknown";
	}
	std::cout << "Mode: " << mode << " - " << modeText << std::endl;
	std::cout << "User Id: " << userId << " Position: " << posX << "," << posY << " Value: " << value << std::endl;

	std::cout << std::endl;
}