#ifndef __PNPCACTION_H
#define __PNPCACTION_H
#include "Packet.h"

class pNpcAction : public Packet {
public:
	int option, mode, val1;

	pNpcAction(int option, int mode, int val1);
	~pNpcAction();

	void pNpcAction::debugPrint();
};

#endif