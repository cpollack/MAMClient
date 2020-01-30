#include "stdafx.h"
#include "Client.h"
#include "pBattleState.h"

#include "GameMap.h"
#include "Battle.h"
#include "Player.h"
#include "MainWindow.h"

pBattleState::pBattleState(char* buf, char* encBuf) {
	description = "Battle State (Server)";
	type = ptBattleState;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getByte(0, &state);
	getByte(1, &actors);
	getInt(4, &userId);
	getInt(8, &formation);
}


pBattleState::pBattleState(int code, int tSize, int uId, int form) {
	description = "Battle State (Client)";
	type = ptBattleState;
	initBuffer(16);

	state = code;
	actors = tSize;
	if (actors == 0) actors = 1;
	userId = uId;
	formation = form;

	addByte(0, state);
	addByte(1, actors);
	addDWord(4, userId);
	addDWord(8, formation);
}


pBattleState::~pBattleState() {

}


void pBattleState::process() {
	if (state == bsConfirmStart) {
		if (userId == player->GetID()) {
			if (map->doesBattleResultExist()) map->deleteBattleResult();

			if (battle) delete battle;
			battle = new Battle(mainForm->renderer, map->getMapDoc(), actors);
			battle->setAllyFormation(gClient.battleFormation);
			battle->setEnemyFormation(formation);
		}
		else {
			//user manager
		}
	}

	if (state == bsEnd) {
		//do anything?
		//battle ends when victory pose is done, which we can tell from round end as well...
		//maybe battle got cancelled for other reasons (Jail?), this can check a flag in Battle and kill the battle when needed
	}
}


void pBattleState::debugPrint() {
	Packet::debugPrint();

	std::cout << "State: " << state << " Actors: " << actors <<  " User ID: " << userId << " Formation: " << formation << std::endl;

	std::cout << std::endl;
}