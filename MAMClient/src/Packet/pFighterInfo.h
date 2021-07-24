#ifndef __PFIGHTERINFO_H
#define __PFIGHTERINFO_H
#include "Packet.h"

class pFighterInfo : public Packet {
public:
	int fiType = 0; // 201 = player + pet, 200 = player
	int userId = 0;
	int look=0, level=0;
	int hp_cur=0, hp_max=0, mana_cur=0, mana_max=0;
	char name[16] = { 0 };

	unsigned int petId=0;
	int petLook=0, petLevel=0;
	int pet_hp_cur=0, pet_hp_max=0;
	char petName[16] = { 0 };

	pFighterInfo(int pSize, char *buf, char* encBuf);
	~pFighterInfo();

	virtual void process();
	void pFighterInfo::debugPrint();
};

#endif