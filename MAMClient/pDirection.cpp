#include "stdafx.h"
#include "Client.h"
#include "pDirection.h"

#include "UserManager.h"
#include "User.h"


pDirection::pDirection(char *buf, char* encBuf) {
	description = "Direction (Server)";
	type = ptDirection;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getWord(4, &x);
	getWord(6, &y);
	getInt(8, &direction);
}

pDirection::pDirection(int uId, int posX, int posY, int dir) {
	description = "Direction (Client)";
	type = ptDirection;
	initBuffer(16);

	userId = uId;
	x = posX;
	y = posY;
	direction = dir;

	addDWord(0, userId);
	addWord(4, x);
	addWord(6, y);
	addDWord(8, direction);
}


pDirection::~pDirection() {

}


void pDirection::process() {
	User* user = userManager.getUserById(userId);
	if (user) {
		user->setDirection(direction);
		user->loadSprite();
	}
}

void pDirection::debugPrint() {
	Packet::debugPrint();

	std::cout << "User Id: " << userId << " Position: " << x << "," << y << " Direction: " << direction << std::endl;

	std::cout << std::endl;
}