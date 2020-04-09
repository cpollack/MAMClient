#ifndef __PBATTLESTATE_H
#define __PBATTLESTATE_H
#include "Packet.h"


typedef enum BattleState {
	bsStart = 0,
	bsUnknown1 = 1, //pk battle
	bsConfirmStart = 2, //npc battle
	bsEnd = 3,
	bsUnknown2 = 6
}BattleState;


class pBattleState : public Packet {
public:
	int state, actors;
	int userId;
	int formation;

	pBattleState(char *buf, char* encBuf);
	pBattleState(int code, int aSize, int uId, int form);
	~pBattleState();

	virtual void process();
	void pBattleState::debugPrint();
};

#endif