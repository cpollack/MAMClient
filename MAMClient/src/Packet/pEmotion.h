#pragma once

#include "Packet.h"

class pEmotion : public Packet {
public:
	int sourceId;
	int x, y;
	int emotion;

	pEmotion(char *buf, char* encBuf);
	~pEmotion();

	virtual void process();
	void debugPrint();
};
