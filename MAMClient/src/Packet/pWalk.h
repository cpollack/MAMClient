#ifndef __PWALK_H
#define __PWALK_H
#include "Packet.h"

class pWalk : public Packet {
public:
	int userId;
	int posX, posY, destX, destY;

	pWalk(char *buf, char* encBuf);
	pWalk(int uId, int fromX, int fromY, int toX, int toY);
	~pWalk();

	virtual void process();
	void pWalk::debugPrint();
};

#endif