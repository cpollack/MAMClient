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
	initBuffer(sizeof(MSG_Info) + 4);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	m_pInfo = new MSG_Info;
	memcpy(m_pInfo, buf + 4, size);
	/*getByte(0, &state);
	getByte(1, &actors);
	getInt(4, &userId);
	getInt(8, &formation);*/

	allyFormation = m_pInfo->dwFormation % 0xFFFF;
	enemyFormation = m_pInfo->dwFormation / 0xFFFF;
}


pBattleState::pBattleState(int state, int actors, int idUser, int formation, int idEnemy) {
	description = "Battle State (Client)";
	type = ptBattleState;
	initBuffer(sizeof(MSG_Info) + 4);

	m_pInfo = new MSG_Info;
	m_pInfo->bState = state;
	m_pInfo->bActors = actors;
	if (m_pInfo->bActors == 0) m_pInfo->bActors = 1;
	m_pInfo->idUser = idUser;
	m_pInfo->dwFormation = formation;
	m_pInfo->idEnemy = idEnemy;

	addBytes(0, (BYTE*)m_pInfo, sizeof(MSG_Info));

	/*addByte(0, state);
	addByte(1, actors);
	addDWord(4, userId);
	addDWord(8, allyFormation);*/
}


pBattleState::~pBattleState() {

}


void pBattleState::process() {
	if (m_pInfo->bState == bsConfirmStart) {
		if (m_pInfo->idUser == player->GetID()) {
			if (map->doesBattleResultExist()) map->deleteBattleResult();

			if (battle) delete battle;
			battle = new Battle(mainForm->renderer, map->getMapDoc(), m_pInfo->bActors);
			battle->setAllyFormation(allyFormation);
			battle->setEnemyFormation(enemyFormation);
		}
		else {
			//user manager
			User *pUser = userManager.getUserById(m_pInfo->idUser);
			CTeam *pTeam = player->GetTeam();
			if (pTeam && pUser->GetID() == pTeam->GetLeader()->GetID()) {
				//my team, intiate battle
				if (map->doesBattleResultExist()) map->deleteBattleResult();

				if (battle) delete battle;
				battle = new Battle(mainForm->renderer, map->getMapDoc(), m_pInfo->bActors);
				battle->setAllyFormation(allyFormation);
				battle->setEnemyFormation(enemyFormation);
			}
			else {
				//not my team, update states
			}
		}
	}

	if (m_pInfo->bState == bsEnd) {
		//do anything?
		//battle ends when victory pose is done, which we can tell from round end as well...
		//maybe battle got cancelled for other reasons (Jail?), this can check a flag in Battle and kill the battle when needed
	}
}


void pBattleState::debugPrint() {
	Packet::debugPrint();

	std::cout << "State: " << m_pInfo->bState << " Actors: " << m_pInfo->bActors <<  " User ID: " << m_pInfo->idUser << " Ally Formation: " << allyFormation << " Enemy Formation: " << enemyFormation << std::endl;

	std::cout << std::endl;
}