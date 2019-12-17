#include "stdafx.h"
#include "Client.h"
#include "pBattleResult.h"

#include "GameMap.h"
#include "Battle.h"
#include "BattleResult.h"
#include "MainWindow.h"

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
	map->addBattleResult(new BattleResult(mainForm->renderer, victory, cash, player_exp, pet_exp, mainWindowRect.x + (mainWindowRect.w / 2), mainWindowRect.y + (mainWindowRect.h / 2)));

	mainForm->setPlayerHealthGauge(player_hp);
	mainForm->setPlayerManaGauge(player_mana);
	mainForm->setPetHealthGauge(pet_hp);

	//update exp guages
	//update pet loyalty
}


void pBattleResult::debugPrint() {
	Packet::debugPrint();

	std::cout << "Result Mode: " << resultMode << "(What does this do?)" << std::endl;
	std::cout << "Exp: " << player_exp << " HP: " << player_hp << " Mana: " << player_mana << " Cash: " << cash << " Reputation: " << reputation << std::endl;
	std::cout << "Pet Exp: " << pet_exp << " Pet HP: " << pet_hp << " Loyalty: " << pet_loyalty << std::endl;

	std::cout << std::endl;
}