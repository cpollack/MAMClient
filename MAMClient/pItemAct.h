#pragma once
#include "Packet.h"


class pItemAct : public Packet {
public:
	DWORD sourceId, targetId;
	WORD battleGroup;
	WORD itemMode;
	BYTE arState[10];
	WORD arLife[10];
	WORD arMana[10];
	WORD sourceState;

	pItemAct(char *buf, char* encBuf);
	~pItemAct();

	virtual void process();
	void debugPrint();
};
