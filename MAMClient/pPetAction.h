#ifndef __PPETACTION_H
#define __PPETACTION_H
#include "Packet.h"

enum PetAction {
	paSetActive = 0,
	paDrop = 3,
	paLearnSkill = 4,
	paForgetSkill = 5,
	paUseItem = 7
	//12 - checkout pet ?
	//15 - related to wrestle
	//16 - wrestle
	//17 - wrestle ?
	//18 - next 3 related pet update info
	//19
	//20
	//21 - checkin pet ?
	//22
	//25
	//35 - pet treasure ?
};

class pPetAction : public Packet {
public:
	int petId, targetId, action;

	pPetAction(char *buf, char* encBuf);
	pPetAction(int pId, int tId, int act);
	~pPetAction();

	virtual void process();
	void pPetAction::debugPrint();
};

#endif