#pragma once

enum TEAM_ACT {
	TEAM_ACT_CREATE = 7758,
	TEAM_ACT_JOIN = 7761,
	TEAM_ACT_KICK = 7764,
	TEAM_ACT_QUIT = 7770,
	TEAM_ACT_LOCK = 7771,
	TEAM_ACT_UNLOCK = 7772,
};

#include "Packet.h"

class pTeam : public Packet {
public:
	int action;
	int sourceId;
	int targetId;

	pTeam(char *buf, char* encBuf);
	pTeam(int act, int sId, int tId);
	~pTeam();

	virtual void process();
	void debugPrint();
};
