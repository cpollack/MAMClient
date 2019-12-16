#ifndef __PTICK_H
#define __PTICK_H
#include "Packet.h"

class pTick : public Packet {
public:
	int userId;
	DWORD systemTime;

	pTick(char *buf, char* encBuf);
	pTick(int uId);
	~pTick();

	virtual void process();
	void pTick::debugPrint();
};

#endif