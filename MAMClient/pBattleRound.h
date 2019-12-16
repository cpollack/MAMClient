#ifndef __PBATTLEROUND_H
#define __PBATTLEROUND_H
#include "Packet.h"


class pBattleRound : public Packet {
public:
	bool alive[20];
	int count;

	pBattleRound(char *buf, char* encBuf);
	~pBattleRound();

	virtual void process();
	void pBattleRound::debugPrint();
};

#endif