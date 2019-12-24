#include "stdafx.h"
#include "pAttributeUpdate.h"

pAttributeUpdate::pAttributeUpdate(int life, int mana, int attack, int defence, int dexterity) {
	description = "Attribute Update (Client)";
	type = ptAttributeUpdate;
	initBuffer(14);

	this->life = life;
	this->mana = mana;
	this->attack = attack;
	this->defence = defence;
	this->dexterity = dexterity;

	addWord(0, life);
	addWord(2, defence);
	addWord(4, attack);
	addWord(6, dexterity);
	addWord(8, mana);
}


pAttributeUpdate::~pAttributeUpdate() {
	//
}


void pAttributeUpdate::process() {
	//
}


void pAttributeUpdate::debugPrint() {
	Packet::debugPrint();

	std::cout << "Life: " << life << " Mana: " << mana << " Attack: " << attack << " Defence: " << defence << " Dex: " << dexterity << std::endl;

	std::cout << std::endl;
}