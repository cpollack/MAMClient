#include "stdafx.h"
#include "Client.h"
#include "pColosseum.h"

#include "GameMap.h"


pColosseum::pColosseum(char *buf, char* encBuf) {
	description = "Colosseum (Server)";
	type = ptColosseum;
	initBuffer(26);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &colId);
	getInt(4, &posX);
	getInt(8, &posY);

	getWord(12, &maxLevel);
	getWord(14, &maxLevel);
	getWord(16, &managerLook);
	getWord(18, &u1);
	getWord(20, &u2);
}


pColosseum::~pColosseum() {

}


void pColosseum::process() {
	map->addColosseum(this);
}


void pColosseum::debugPrint() {
	Packet::debugPrint();

	std::cout << "Colosseum ID: " << colId << " Manager: " << managerLook << " Pos: " << posX << "," << posY << std::endl;
	std::cout << "Max Lvl: " << maxLevel << " Min Lvl: " << minLevel << " Unknown 1/2: " << u1 << " / " << u2 << std::endl;

	std::cout << std::endl;
}