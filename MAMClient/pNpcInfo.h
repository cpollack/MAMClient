#ifndef __PNPCINFO_H
#define __PNPCINFO_H
#include "Packet.h"


class pNpcInfo : public Packet {
public:
	int id=0;
	int hslType=0;
	int npcType=0, look=0;
	int posX=0, posY=0;
	char name[16] = { 0 };
	BYTE hslSets[15] = { 0 };

	pNpcInfo(char *buf, char* encBuf);
	~pNpcInfo();

	virtual void process();
	void pNpcInfo::debugPrint();
};

#endif