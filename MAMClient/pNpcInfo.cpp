#include "stdafx.h"
#include "Client.h"
#include "pNpcInfo.h"

#include "GameMap.h"


pNpcInfo::pNpcInfo(char *buf, char* encBuf) {
	description = "NPC Info (Client)";
	type = ptNpcInfo;
	initBuffer(52);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &id);
	getWord(4, &hslType);
	getWord(6, &npcType);
	getWord(8, &look);
	getWord(10, &posX);
	getWord(12, &posY);

	getString(14, name, 16);
	getString(30, (char*)hslSets, 15);
}


pNpcInfo::~pNpcInfo() {

}


void pNpcInfo::process() {
	map->addNpc(this);
}


void pNpcInfo::debugPrint() {
	Packet::debugPrint();

	std::cout << "NPC ID: " << id << " Type: " << type << " Look: " << look << " Position: " << posX << "," << posY << std::endl;
	std::cout << name << " - HSL Type: " << hslType << " Vals: " << hslSets << std::endl;

	std::cout << std::endl;
}