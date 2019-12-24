#pragma once

#include "Packet.h"

class pAttributeUpdate : public Packet {
public:
	int life, mana, attack, defence, dexterity;

	pAttributeUpdate(int life, int mana, int attack, int defence, int dexterity);
	~pAttributeUpdate();

	virtual void process();
	void debugPrint();
};
