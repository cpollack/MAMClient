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

	SDL_Event e;
	SDL_zero(e);
	switch (action) {
	case paSetActive:
		activePet = player->setActivePet(petId);

		SDL_Event e;
		SDL_zero(e);
		e.type = CUSTOMEVENT_PET;
		e.user.code = PET_MARCHING;
		SDL_PushEvent(&e);
		break;

	case paUseItem:
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_USE;
		e.user.data1 = player->inventory->getItem(value);
		SDL_PushEvent(&e);

		activePet = player->getPet(petId);
		activePet->useItem(value);
		break;

	case paFullHeal:
		activePet = player->getActivePet();
		if (activePet) {
			activePet->SetLife(value);

			SDL_Event e;
			SDL_zero(e);
			e.type = CUSTOMEVENT_PET;
			e.user.code = PET_LIFE;
			SDL_PushEvent(&e);
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