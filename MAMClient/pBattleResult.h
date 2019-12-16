#ifndef __PBATTLERESULT_H
#define __PBATTLERESULT_H
#include "Packet.h"

enum BattleResultMode {
	brmFlee = 0, //after run
	brmVictory = 1,
	brmTeamVictory = 2, //best guess
	brmDefeat = 3, //confirm after ko
	brmTeamDefeat = 4 //best guess again
};

class pBattleResult : public Packet {
public:
	int resultMode;
	int player_exp, player_hp, player_mana;
	int cash, reputation;
	int pet_exp, pet_hp, pet_loyalty;

	pBattleResult(char *buf, char* encBuf);
	~pBattleResult();

	virtual void process();
	void pBattleResult::debugPrint();
};

#endif