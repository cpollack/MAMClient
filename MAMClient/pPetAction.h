#ifndef __PPETACTION_H
#define __PPETACTION_H
#include "Packet.h"

enum PetAction {
	paSetActive = 0,
	paDrop = 3,
	paLearnSkill = 4,
	paForgetSkill = 5,
	paUseItem = 7,
	//12 - checkout pet ?
	//15 - related to wrestle
	//16 - wrestle
	//17 - wrestle ?
	paFullHeal = 18,
	//19
	//20
	paRemove = 21,
	//22 ?
	//25
	//35 - pet treasure ?
};

class pPetAction : public Packet {
public:
	int petId, action, value;

	pPetAction(char *buf, char* encBuf);
	pPetAction(int pId, int tId, int act);
	~pPetAction();

	virtual void process();
	void pPetAction::debugPrint();
};

#endif