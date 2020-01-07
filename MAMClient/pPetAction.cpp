#include "stdafx.h"
#include "Client.h"
#include "pPetAction.h"

#include "CustomEvents.h"

#include "Player.h"
#include "Pet.h"
#include "MainWindow.h"


pPetAction::pPetAction(char *buf, char* encBuf) {
	description = "Pet Action (Server)";
	type = ptPetAction;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &petId);
	getInt(4, &targetId);
	getInt(8, &action);
}


pPetAction::pPetAction(int pId, int tId, int act) {
	description = "Pet Action (Client)";
	type = ptPetAction;
	initBuffer(16);

	petId = pId;
	targetId = tId;
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

		SDL_Event e;
		SDL_zero(e);
		e.type = CUSTOMEVENT_PET;
		e.user.code = PET_MARCHING;
		SDL_PushEvent(&e);

		break;
	case paUseItem:
		activePet = player->getPet(petId);
		activePet->useItem(targetId);
		break;
	}
}


void pPetAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Pet ID: " << petId << " Target ID: " << targetId << " Action: " << action << std::endl;

	std::cout << std::endl;
}