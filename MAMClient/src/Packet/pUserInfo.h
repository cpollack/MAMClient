#pragma once

#include "Packet.h"

enum UserInfoEmotion {
	uieNone = 0,
	uieWave = 1,
	uieFaint = 2,
	uieSit = 3,
	uieKneel = 4,
	//unknown = 5,
	uieHappy = 6,
	uieBow = 7,
	uieFly = 8, //Tentative, need to confirm
};

class pUserInfo : public Packet {
public:
	int userId;
	int x, y;
	int look;
	int face, state, direction, emotion;
	int rank, reborns, rankType, masterType, alignment;
	int level;
	int pkEnabled;
	int syndicateId, subGroudId, syndicateRank;
	BYTE colorSets[25];
	int z1, z2;

	std::string name, nickName, spouse, guild, subGroup, guildTitle;

	pUserInfo(int pSize, char *buf, char* encBuf);
	~pUserInfo();

	virtual void process();
	void debugPrint();
};
