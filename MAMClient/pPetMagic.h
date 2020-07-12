#pragma once
#include "Packet.h"
#include "Define.h"

enum PETMAGIC_MODE {
	pmEvolve = 0,
	pmCompose = 1,
};

struct PET_MAGIC_ENTRY {
	DWORD look;
	ColorShift shifts[3];
	BYTE buffer;
};

class pPetMagic : public Packet {
public:
	int mode, playerId, count;
	PET_MAGIC_ENTRY entry[4];

	pPetMagic(char *buf, char* encBuf);
	~pPetMagic();

	virtual void process();
	void debugPrint();
};
