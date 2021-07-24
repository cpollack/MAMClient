#ifndef __PROLE_H
#define __PROLE_H

#include "Packet.h"

class pRole : public Packet {
public:
	int look, face;
	int mapId;
	int point_life, point_mana, point_attack, point_def, point_dex;
	BYTE name[16] = { 0 };
	BYTE nickName[16] = { 0 };
	BYTE hsl[25] = { 0 };

	pRole(char *buf, char* encBuf);
	pRole(int aLook, int aFace, int aMapId, int life, int mana, int attack, int def, int dex, char* aName, char* aNickName, char* hslSets);
	~pRole();

	virtual void process();
	void pRole::debugPrint();
};

#endif