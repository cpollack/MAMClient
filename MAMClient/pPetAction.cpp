#include "stdafx.h"
#include "Client.h"
#include "pPetAction.h"

#include "CustomEvents.h"

#include "Player.h"
#include "Pet.h"
#include "Inventory.h"
#include "MainWindow.h"


pPetAction::pPetAction(char *buf, char* encBuf) {
	description = "Pet Action (Server)";
	type = ptPetAction;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &petId);
	getInt(4, &value);
	getInt(8, &action);
}


pPetAction::pPetAction(int pId, int tId, int act) {
	description = "Pet Action (Client)";
	type = ptPetAction;
	initBuffer(16);

	petId = pId;
	value = tId;
	action = act;

	addDWord(0, pId);
	addDWord(4, tId);
	addDWord(8, act);
}


pPetAction::~pPetAction() {

}


void pPetAction::process() {
	Pet* activePet;

	switch (action) {
	case paSetActive:
		activePet = player->setActivePet(petId);
		customEvent(CUSTOMEVENT_PET, PET_MARCHING);
		break;

	case paUseItem:
		activePet = player->getPet(petId);
		activePet->useItem(value);
		customEvent(CUSTOMEVENT_ITEM, ITEM_USE, player->inventory->getItem(value));
		break;

	case paFullHeal:
		activePet = player->getActivePet();
		if (activePet) {
			activePet->SetLife(value);
			customEvent(CUSTOMEVENT_PET, PET_LIFE);
		}
		break;

	case paRemove:
		player->removePet(value);
		break;
	}
}


void pPetAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Pet ID: " << petId << " Value: " << value << " Action: " << action << std::endl;

	std::cout << std::endl;
}