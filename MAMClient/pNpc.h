#ifndef __PNPC_H
#define __PNPC_H
#include "Packet.h"

class pNpc : public Packet {
public:
	int id = 0;
	int npcType = 0;

	pNpc(int npcId, int npcType, int val3, int val4);
	~pNpc();

	void pNpc::debugPrint();
};

#endif