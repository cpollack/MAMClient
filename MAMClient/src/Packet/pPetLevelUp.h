#pragma once

#include "Packet.h"

class pPetLevelUp : public Packet {
public:
	DWORD petId;
	int level;
	int attack;
	int defence;
	int dexterity;
	int life;
	int loyalty;

	pPetLevelUp(char *buf, char* encBuf);
	~pPetLevelUp();

	virtual void process();
	void debugPrint();
};
