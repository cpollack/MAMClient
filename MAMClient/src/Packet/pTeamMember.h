#pragma once

#include "Packet.h"

class pTeamMember : public Packet {
public:
	int memberId[5];

	pTeamMember(char *buf, char* encBuf);
	~pTeamMember();

	virtual void process();
	void debugPrint();
};
