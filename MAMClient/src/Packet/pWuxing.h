#pragma once

#include "Packet.h"

typedef enum WUX_MODE {
	WUX_MODE_PREVIEW = 0,
	WUX_MODE_START = 1,
};

typedef struct PACKET_WUX_SLOT {
	DWORD type;
	DWORD id;
};

class pWuxing : public Packet {
public:
	int mode;
	int mainIngredient;
	PACKET_WUX_SLOT slots[8];

	pWuxing(char *buf, char* encBuf);
	pWuxing(int iMode, int iMain, std::vector<PACKET_WUX_SLOT> vSlots);
	~pWuxing();

	virtual void process();
	void debugPrint();
};