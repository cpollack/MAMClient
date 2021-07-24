#ifndef __PPLAYERINFO_H
#define __PPLAYERINFO_H

#include "Packet.h"

class pPlayerInfo : public Packet {
public:
	int userId;
	int look, face;
	int level, experience, life_current, life_max, mana_current, mana_max;
	int rank, reborns, masterType, alignment;
	int cash, reputation, cultivation;
	bool pkEnabled;
	int wuxing, kungfu, petRaising, thievery;
	int unused_point, life_point, defence_point, attack_point, dexterity_point, mana_point;

	int mapId, mapX, mapY;
	int syndicateId, syndicateSubId, syndicateRank;

	char name[16];
	char nickName[16];
	char spouse[16];
	char syndicate[16];
	char branch[16];
	char position[16];

	pPlayerInfo(int size,  char *buf, char* encBuf);
	pPlayerInfo();
	~pPlayerInfo();

	virtual void process();
	void pPlayerInfo::debugPrint();
};

#endif