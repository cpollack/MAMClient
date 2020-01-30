#include "stdafx.h"
#include "pPetLevelUp.h"

#include "CustomEvents.h"
#include "BattleResult.h"
#include "Player.h"
#include "Pet.h"

pPetLevelUp::pPetLevelUp(char* buf, char* encBuf) {
	description = "Pet Levelup (Server)";
	type = ptPetLevelUp;
	initBuffer(20);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getDWord(0, &petId);
	getWord(4, &level);
	getWord(6, &attack);
	getWord(8, &defence);
	getWord(10, &dexterity);
	getWord(12, &life);
	getWord(14, &loyalty);
}


pPetLevelUp::~pPetLevelUp() {
	//
}


void pPetLevelUp::process() {
	Pet* pet = player->getPet(petId);
	if (!pet) return;

	pet->SetLevel(level);
	pet->SetExperience(0);
	pet->SetAttack(attack);
	pet->SetDefence(defence);
	pet->SetDexterity(dexterity);
	pet->SetMaxLife(life);
	pet->SetLoyalty(loyalty);

	Pet* activePet = player->getActivePet();
	if (activePet && activePet->GetID() == petId) {
		SDL_Event e;
		SDL_zero(e);
		e.type = CUSTOMEVENT_PET;
		e.user.code = PET_LEVEL;
		SDL_PushEvent(&e);

		PetLeveled = true;
	}
}


void pPetLevelUp::debugPrint() {
	Packet::debugPrint();

	std::cout << "Pet Id:" << petId << " Level:" << level << " 3A:" << attack << "," << defence<< "," << dexterity << " Life:" << life << " Loyalty:" << loyalty << std::endl;

	std::cout << std::endl;
}