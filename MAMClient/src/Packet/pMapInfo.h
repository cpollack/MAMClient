#ifndef __PMAPINFO_H
#define __PMAPINFO_H

#include "Packet.h"

class pMapInfo : public Packet {
public:
	int verifNum;
	int userId;
	int mapId, mapDoc, mapType, mapX, mapY;
	int weather, bgMusic, bgMusic_show, fightMusic;

	char mapName[16] = { 0 };

	pMapInfo(char* buf, char* encBuf);
	~pMapInfo();

	virtual void process();
	void pMapInfo::debugPrint();
};

#endif