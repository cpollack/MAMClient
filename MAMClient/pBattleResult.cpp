#include "stdafx.h"
#include "Client.h"
#include "pBattleResult.h"

#include "CustomEvents.h"

#include "Player.h"
#include "Pet.h"
#include "GameMap.h"
#include "Battle.h"
#include "BattleResult.h"

pBattleResult::pBattleResult(char* buf, char* encBuf) {
	description = "Battle Result (Server)";
	type = ptBattleResult;
	initBuffer(32);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getWord(0, &resultMode);
	getWord(2, &cash);
	getWord(4, &player_hp);
	getWord(6, &player_mana);
	getInt(8, &reputation);
	getInt(12, &player_exp);
	getWord(16, &pet_hp);
	getWord(18, &pet_loyalty);
	getInt(20, &pet_exp);
}


pBattleResult::~pBattleResult() {

}


void pBattleResult::process() {
	battle->handlePacket(this);
	if (map->doesBattleResultExist()) map->deleteBattleResult();

	SDL_Rect mainWindowRect = gui->mainWindow->rect;
	bool victory = false;
	if (resultMode == brmVictory) victory = true;
	map->addBattleResult(new BattleResult(map->renderer, victory, cash, player_exp, pet_exp, mainWindowRect.x + (mainWindowRect.w / 2), mainWindowRect.y + (mainWindowRect.h / 2)));

	SDL_Event e;
	SDL_zero(e);

	e.type = CUSTOMEVENT_PLAYER;
	if (cash > 0) {
		player->SetCash(player->GetCash() + cash);

		e.user.code = PLAYER_MONEY;
		SDL_PushEvent(&e);
	}

	player->SetReputation(reputation);
	e.user.code = PLAYER_REPUTATION;
	SDL_PushEvent(&e);

	player->SetLife(player_hp);
	e.user.code = PLAYER_LIFE;
	SDL_PushEvent(&e);

	player->SetMana(player_mana);
	e.user.code = PLAYER_MANA;
	SDL_PushEvent(&e);

	if (PlayerLeveled) PlayerLeveled = false;
	else {
		player->SetExperience(player->GetExperience() + player_exp);
		e.user.code = PLAYER_EXP;
		SDL_PushEvent(&e);
	}

	e.type = CUSTOMEVENT_PET;
	Pet* pet = player->getActivePet();
	if (pet) {
		pet->SetLife(pet_hp);
		e.user.code = PET_LIFE;
		SDL_PushEvent(&e);

		//Verify the pet didn't level post battle before receiving battle results
		if (PetLeveled) PetLeveled = false;
		else {
			pet->SetExperience(pet->getExperience() + pet_exp);
			e.user.code = PET_EXP;
			SDL_PushEvent(&e);
		}

		pet->SetLoyalty(pet_loyalty);
	}
}


void pBattleResult::debugPrint() {
	Packet::debugPrint();

	std::cout << "Result Mode: " << resultMode << "(What does this do?)" << std::endl;
	std::cout << "Exp: " << player_exp << " HP: " << player_hp << " Mana: " << player_mana << " Cash: " << cash << " Reputation: " << reputation << std::endl;
	std::cout << "Pet Exp: " << pet_exp << " Pet HP: " << pet_hp << " Loyalty: " << pet_loyalty << std::endl;

	std::cout << std::endl;
}