#pragma once

#include "Packet.h"

class pUserPoint : public Packet {
public:
	int life, mana, attack, defence, dexterity;

	pUserPoint(int life, int mana, int attack, int defence, int dexterity);
	~pUserPoint();

	virtual void process();
	void debugPrint();
};
