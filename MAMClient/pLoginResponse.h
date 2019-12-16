#ifndef __PLOGINRESPONSE_H
#define __PLOGINRESPONSE_H

#include "Packet.h"

class pLoginResponse : public Packet {
public:
	int accountID;
	int seed;
	BYTE phrase[16] = { 0 };

	pLoginResponse(char *buf, char* encBuf);
	pLoginResponse(int acctID, int sd);
	~pLoginResponse();

	virtual void process();
	void pLoginResponse::debugPrint();
};

#endif