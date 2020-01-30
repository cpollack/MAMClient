#pragma once

#include "Packet.h"

class pUserLevelUp : public Packet {
public:
	DWORD userId;
	int level;
	int experience;
	int attributePoints;

	pUserLevelUp(char *buf, char* encBuf);
	~pUserLevelUp();

	virtual void process();
	void debugPrint();
};
