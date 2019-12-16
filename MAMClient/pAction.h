#pragma once

#include "Packet.h"

class pAction : public Packet {
public:
	int sourceId;
	int value1, value2, value3, value4;

	pAction(char *buf, char* encBuf);
	~pAction();

	virtual void process();
	void debugPrint();
};
