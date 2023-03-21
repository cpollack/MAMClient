#ifndef __PBATTLESTATE_H
#define __PBATTLESTATE_H
#include "Packet.h"


typedef enum BattleState {
	bsStart = 0,
	bsUnknown1 = 1, //pk battle
	bsConfirmStart = 2, //npc battle
	bsEnd = 3,
	//bsUnknown1 = 4,
	//bsUnknown2 = 6
}BattleState;


class pBattleState : public Packet {
public:
	typedef struct
	{
		BYTE bState;
		BYTE bActors;
		DWORD idUser;
		DWORD dwFormation;
		DWORD idEnemy;
	}MSG_Info;
	MSG_Info* m_pInfo;

	int allyFormation, enemyFormation;

	pBattleState(char *buf, char* encBuf);
	pBattleState(int state, int actors, int idUser, int formation, int idEnemy);
	~pBattleState();

	virtual void process();
	void pBattleState::debugPrint();
};

#endif