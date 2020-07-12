#include "stdafx.h"
#include "pPetMagic.h"

#include "UserManager.h"
#include "User.h"
#include "PetMagic.h"
#include "MainWindow.h"
#include "GameMap.h"

pPetMagic::pPetMagic(char *buf, char* encBuf) {
	description = "Pet Magic (Server)";
	type = ptPetAction;
	initBuffer(96);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &mode);
	getInt(4, &playerId);
	getInt(8, &count);

	int pos = 12;
	for (int i = 0; i < count; i++) {
		PET_MAGIC_ENTRY next;
		getArray(pos, (BYTE*)&next, sizeof(PET_MAGIC_ENTRY));
		entry[i] = next;
		pos += sizeof(PET_MAGIC_ENTRY);
	}
}

pPetMagic::~pPetMagic() {
	//
}

void pPetMagic::process() {
	User *pUser = userManager.getUserById(playerId);
	if (!pUser) return;

	
	CPetMagic *pm = new CPetMagic(mainForm->renderer, mode);
	pm->setCoordinate(pUser->GetCoord());

	for (int i = 0; i < 4; i++) {
		if (entry[i].look == 0)  break;

		if (mode == pmEvolve) {
			if (i < 1) {
				pm->addSource(entry[i].look, entry[i].shifts);
			}
			else {
				pm->addDestination(entry[i].look, entry[i].shifts);
			}
		}
		if (mode == pmCompose) {
			if (i < 2) {
				pm->addSource(entry[i].look, entry[i].shifts);
			}
			else {
				pm->addDestination(entry[i].look, entry[i].shifts);
			}
		}
	}
	map->addPetMagic(pm);
	pm->start();
}

void pPetMagic::debugPrint() {
	Packet::debugPrint();

	std::cout << "Mode: " << mode << " PlayerID: " << playerId << " Count: " << count << std::endl;

	std::cout << std::endl;
}