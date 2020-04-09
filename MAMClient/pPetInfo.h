#ifndef __PPETINFO_H
#define __PPETINFO_H

#include "Packet.h"

enum PetInfoMode {
	pimAdd = 0,
	pimUpdate = 1,
	//2 - retrieve from storage?
	//3 - trading?
	//4 - related to pet shop
	//6 - cpacknpc checkin
	//9 - move to storage or query storaged pet
};

class pPetInfo : public Packet {
public:
	int mode;

	int petId, ownerId;
	char name[16];

	int monsterClass, look;
	int attack, defence, dexterity;
	int level, experience, generation, loyalty;
	int life_current, life_max;
	int skillCount;
	int skills[5];
	BYTE val1, val2, val3, val4;
	int medal_attack, medal_defence, medal_dexterity;

	BYTE hslSets[15];

	pPetInfo(char *buf, char* encBuf);
	~pPetInfo();

	virtual void process();
	void pPetInfo::debugPrint();
};

#endif