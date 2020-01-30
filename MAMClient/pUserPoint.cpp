#include "stdafx.h"
#include "pUserPoint.h"

pUserPoint::pUserPoint(int life, int mana, int attack, int defence, int dexterity) {
	description = "User Point Update (Client)";
	type = ptUserPoint;
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


pUserPoint::~pUserPoint() {
	//
}


void pUserPoint::process() {
	//
}


void pUserPoint::debugPrint() {
	Packet::debugPrint();

	std::cout << "Life: " << life << " Mana: " << mana << " Attack: " << attack << " Defence: " << defence << " Dex: " << dexterity << std::endl;

	std::cout << std::endl;
}