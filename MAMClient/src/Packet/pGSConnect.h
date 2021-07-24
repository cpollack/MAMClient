#ifndef __PGSCONNECT_H
#define __PGSCONNECT_H

#include "Packet.h"

class pGsConnect : public Packet {
public:
	int accountId, seed;
	int responseCode;
	BYTE response[16] = { 0 };

	pGsConnect(char *buf, char* encBuf);
	pGsConnect(int acctId, int sd, int respCode, char* resp);
	~pGsConnect();

	virtual void process();
	void pGsConnect::debugPrint();
};

#endif