#ifndef __PITEMACTION_H
#define __PITEMACTION_H

#include "Packet.h"

enum ItemAction {
	iaUse = 1,
	iaBuy = 2,
	iaSell = 3,
	iaDrop = 4,
	//5 - dont have money ? buying item ?
	//6 - failed during battle
	//7
	//9 - failed not enough mny
	iaUnequip = 10,
	//11 - related to trade
	//12 - related to wuxing
	//14
	iaUpdateCount = 25,
};

class pItemAction : public Packet {
public:
	int itemId, data, action;

	pItemAction(char *buf, char* encBuf);
	pItemAction(int id, int act);
	pItemAction(int id, int data, int act);
	~pItemAction();

	virtual void process();
	void pItemAction::debugPrint();
};

#endif