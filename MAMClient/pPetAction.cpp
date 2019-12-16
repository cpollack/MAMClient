#include "stdafx.h"
#include "Client.h"
#include "pPetAction.h"

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
		if (mainForm->getMode() == MFM_MAIN) {
			mainForm->setPetHealthGauge(activePet->getCurrentHealth(), activePet->getMaxHealth());
			mainForm->setPetExpGauge(activePet->getExperience(), activePet->getLevelUpExperience());

			for (auto form : Windows) {
				if (form->GetType() == FT_PET) {
					//((CPetForm*)form)->setMarchingPet(petId);
					break;
				}
			}
		}
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