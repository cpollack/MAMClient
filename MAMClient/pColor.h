#ifndef __PCOLOR_H
#define __PCOLOR_H
#include "Packet.h"

class pColor : public Packet {
public:
	int sourceId;
	int count;
	BYTE hslSets[25] = { 0 };

	pColor(char *buf, char* encBuf);
	~pColor();

	virtual void process();
	void pColor::debugPrint();
};

#endif