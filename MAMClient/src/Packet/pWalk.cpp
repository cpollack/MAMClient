#include "stdafx.h"
#include "Client.h"
#include "pWalk.h"

#include "UserManager.h"
#include "User.h"

pWalk::pWalk(char *buf, char* encBuf) {
	description = "Walk (Server)";
	type = ptWalk;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getWord(4, &posX);
	getWord(6, &posY);
	getWord(8, &destX);
	getWord(10, &destY);
}


pWalk::pWalk(int uId, int fromX, int fromY, int toX, int toY) {
	description = "Walk (Client)";
	type = ptWalk;
	initBuffer(16);

	userId = uId;
	posX = fromX;
	posY = fromY;
	destX = toX;
	destY = toY;

	addDWord(0, userId);
	addWord(4, posX);
	addWord(6, posY);
	addWord(8, destX);
	addWord(10, destY);
}


pWalk::~pWalk() {

}


void pWalk::process() {
	User* user = userManager.getUserById(userId);
	if (user) user->walkTo(SDL_Point{ destX, destY });
}


void pWalk::debugPrint() {
	Packet::debugPrint();

	std::cout << "User Id: " << userId << " Position: " << posX << "," << posY << " Destination: " << destX << "," << destY << std::endl;

	std::cout << std::endl;
}