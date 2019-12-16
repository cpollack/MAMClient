#ifndef __PBATTLERESPONSE_H
#define __PBATTLERESPONSE_H
#include "Packet.h"


class pBattleResponse : public Packet {
public:
	int action, group;
	bool sourceState, targetState;
	int sourceId, targetId;
	int damage;
	int v1, v2, v3;

	pBattleResponse(char *buf, char* encBuf);
	~pBattleResponse();

	virtual void process();
	void pBattleResponse::debugPrint();
};

#endif