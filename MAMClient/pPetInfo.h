#ifndef __PPETINFO_H
#define __PPETINFO_H

#include "Packet.h"

class pPetInfo : public Packet {
public:
	int val1;

	int petId, ownerId;
	char name[16];

	int monsterClass, look;
	int attack, defence, dexterity;
	int level, experience, generation, loyalty;
	int life_current, life_max;
	int skillCount;
	int skills[5];
	int val2;
	int medal_attack, medal_defence, medal_dexterity;

	BYTE hslSets[15];

	pPetInfo(char *buf, char* encBuf);
	~pPetInfo();

	virtual void process();
	void pPetInfo::debugPrint();
};

#endif