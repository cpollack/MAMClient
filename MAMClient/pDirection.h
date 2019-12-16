#ifndef __PDIRECTION_H
#define __PDIRECTION_H
#include "Packet.h"

class pDirection : public Packet {
public:
	int userId;
	int x, y;
	int direction;

	pDirection(char *buf, char* encBuf);
	pDirection(int uId, int posX, int posY, int dir);
	~pDirection();

	virtual void process();
	void pDirection::debugPrint();
};

#endif