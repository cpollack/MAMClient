#include "stdafx.h"
#include "Client.h"
#include "pFighterInfo.h"

#include "Battle.h"

pFighterInfo::pFighterInfo(int pSize, char* buf, char* encBuf) {
	description = "Battle - Fighter Info (Server)";
	type = ptFighterInfo;
	initBuffer(pSize);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &fiType);

	getInt(4, &userId);
	getWord(8, &look);
	getWord(10, &level);
	getWord(12, &hp_cur);
	getWord(14, &hp_max);
	getWord(16, &mana_cur);
	getWord(18, &mana_max);
	getString(20, name, 16);

	if (fiType == 201) { //201 includes pet
		getInt(36, &petId);
		getWord(40, &petLook);
		getWord(42, &petLevel);
		getWord(44, &pet_hp_cur);
		getWord(46, &pet_hp_max);
		getString(52, petName, 16);
	}

}


pFighterInfo::~pFighterInfo() {

}


void pFighterInfo::process() {
	battle->handlePacket(this);
}

void pFighterInfo::debugPrint() {
	Packet::debugPrint();

	std::cout << "Player: " << userId << " " << name << " - Look: " << look << " Level: " << level;
	std::cout << " Hp: " << hp_cur << "/" << hp_max << " Mana: " << mana_cur << "/" << mana_max << std::endl;

	if (fiType == 201) {
		std::cout << "Pet: " << petId << " " << petName << " - Look: " << petLook << " Level: " << petLevel;
		std::cout << " Hp: " << pet_hp_cur << "/" << pet_hp_max << std::endl;
	}

	std::cout << std::endl;
}