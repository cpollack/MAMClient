#include "stdafx.h"
#include "Client.h"
#include "pPetInfo.h"

#include "Player.h"
#include "Pet.h"

pPetInfo::pPetInfo(char* buf, char* encBuf) {
	description = "Pet Info (Server)";
	type = ptPetInfo;
	initBuffer(104);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &mode);
	getInt(4, &petId);
	getString(8, name, 16);
	getInt(24, &monsterClass);
	getWord(28, &look);
	getWord(30, &attack);
	getWord(32, &defence);
	getWord(34, &dexterity);

	getInt(36, &level);
	getInt(40, &experience);
	getWord(44, &life_current);
	getWord(46, &life_max);
	getByte(48, &generation);

	getByte(49, &skillCount);
	getInt(52, &skills[0]);
	getInt(56, &skills[1]);
	getInt(60, &skills[2]);
	getInt(64, &skills[3]);
	getInt(68, &skills[4]);

	getInt(72, &val2);
	getByte(76, &medal_attack);
	getByte(77, &medal_defence);
	getByte(78, &medal_dexterity);
	getByte(79, &loyalty);

	getInt(80, &ownerId);
	getString(84, (char*)hslSets, 15);
}


pPetInfo::~pPetInfo() {
	//
}


void pPetInfo::process() {
	Pet* pet = nullptr;

	switch (mode) {
	case pimAdd:
		pet = new Pet(this);
		player->addPet(pet);
		break;

	case pimUpdate:
		pet = player->getPet(petId);
		if (pet) pet->updateInfo(this);
		break;
	}
}


void pPetInfo::debugPrint() {
	hideEncryptedBuffer = true;
	Packet::debugPrint();

	std::cout << "Mode:" << mode << " Pet:" << petId << " " << (char*)name << "(" << monsterClass << " - " << look << ")" << std::endl;
	std::cout << "Level: " << level << " Experience: " << experience << " Loyalty: " << loyalty << " Generation: " << generation << std::endl;
	std::cout << "Life: " << life_current << "/" << life_max << " Atk/Def/Dex: " << attack << "/" << defence << "/" << dexterity << " Medals: " << medal_attack << "/" << medal_defence << "/" << medal_dexterity << std::endl;
	std::cout << "Skills: " << skillCount;
	for (int i = 0; i < skillCount; i++)
		std::cout << " Skill" << i << ": " << skills[i];
	std::cout << std::endl;

	std::cout << std::endl;
}