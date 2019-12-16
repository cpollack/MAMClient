#ifndef __PCOLOSSEUM_H
#define __PCOLOSSEUM_H

#include "Packet.h"


class pColosseum : public Packet {
public:
	int colId=0, posX=0, posY=0;
	int maxLevel=0, minLevel=0, managerLook=0, u1=0, u2=0;

	pColosseum(char *buf, char* encBuf);
	~pColosseum();

	virtual void process();

	void pColosseum::debugPrint();
};

#endif