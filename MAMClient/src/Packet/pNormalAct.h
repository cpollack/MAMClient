#pragma once
#include "Packet.h"


class pNormalAct : public Packet {
public:
	int action, group;
	bool sourceState, targetState;
	DWORD sourceId, targetId;
	int damage;
	int interaction;
	int heal, unk;

	pNormalAct(char *buf, char* encBuf);
	~pNormalAct();

	virtual void process();
	void pNormalAct::debugPrint();
};
