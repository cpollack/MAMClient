#include "stdafx.h"
#include "pColor.h"

#include "Battle.h"
#include "UserManager.h"
#include "Player.h"
#include "Pet.h"

pColor::pColor(char* buf, char* encBuf) {
	description = "Color (Server)";
	type = ptColor;
	initBuffer(36);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &sourceId);
	getWord(4, &count);
	if (count > 5) count = 5;

	getString(6, (char*)hslSets, 25);
}


pColor::~pColor() {
	//
}


void pColor::process() {
	if (battle) {
		battle->handleColorPacket(this);
	}
	else {
		//packet applies to a map object
		Entity* entity = nullptr;
		if (player->GetID() == sourceId) entity = player;
		if (!entity && player->getActivePet() && player->getActivePet()->GetID() == sourceId) entity = player->getActivePet();
		if (!entity) entity = userManager.getUserById(sourceId);
		if (entity) {
			ColorShifts colorShifts;
			for (int i = 0; i < 25; i += 5) {
				ColorShift shift;
				memcpy(&shift, hslSets + i, 5);
				colorShifts.push_back(shift);
			}
			if (colorShifts.size() > 0) entity->setColorShifts(colorShifts);
			entity->loadSprite();
		}
	}
}


void pColor::debugPrint() {
	Packet::debugPrint();

	std::cout << "Source Id: " << sourceId << " Count: " << count << std::endl;
	for (int i = 0; i < count; i++) {
		int pos = i * 5;
		std::cout << (int)hslSets[pos + 0] << "," << (int)hslSets[pos + 1] << ","  << (int)hslSets[pos + 2] << ","  << (int)hslSets[pos + 3] << ","  << (int)hslSets[pos + 4] << std::endl;
	}

	std::cout << std::endl;
}