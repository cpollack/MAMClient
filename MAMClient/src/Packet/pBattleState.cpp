#include "stdafx.h"
#include "Client.h"
#include "pBattleState.h"

#include "GameMap.h"
#include "Battle.h"
#include "UserManager.h"
#include "Player.h"
#include "Team.h"

#include "MainWindow.h" //for renderer

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

	allyFormation = formation % 0xFFFF;
	enemyFormation = formation / 0xFFFF;
}


pBattleState::pBattleState(int code, int tSize, int uId, int form) {
	description = "Battle State (Client)";
	type = ptBattleState;
	initBuffer(16);

	state = code;
	actors = tSize;
	if (actors == 0) actors = 1;
	userId = uId;
	allyFormation = form;

	addByte(0, state);
	addByte(1, actors);
	addDWord(4, userId);
	addDWord(8, allyFormation);
}


pBattleState::~pBattleState() {

}


void pBattleState::process() {
	if (state == bsConfirmStart) {
		if (userId == player->GetID()) {
			if (map->doesBattleResultExist()) map->deleteBattleResult();

			if (battle) delete battle;
			battle = new Battle(mainForm->renderer, map->getMapDoc(), actors);
			battle->setAllyFormation(allyFormation);
			battle->setEnemyFormation(enemyFormation);
		}
		else {
			//user manager
			User *pUser = userManager.getUserById(userId);
			CTeam *pTeam = player->GetTeam();
			if (pTeam && pUser->GetID() == pTeam->GetLeader()->GetID()) {
				//my team, intiate battle
				if (map->doesBattleResultExist()) map->deleteBattleResult();

				if (battle) delete battle;
				battle = new Battle(mainForm->renderer, map->getMapDoc(), actors);
				battle->setAllyFormation(allyFormation);
				battle->setEnemyFormation(enemyFormation);
			}
			else {
				//not my team, update states
			}
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

	std::cout << "State: " << state << " Actors: " << actors <<  " User ID: " << userId << " Ally Formation: " << allyFormation << " Enemy Formation: " << enemyFormation << std::endl;

	std::cout << std::endl;
}