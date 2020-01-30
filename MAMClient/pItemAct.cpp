#include "stdafx.h"
#include "Client.h"
#include "pItemAct.h"

#include "Battle.h"

pItemAct::pItemAct(char* buf, char* encBuf) {
	description = "Battle - Item Act (Server)";
	type = ptItemAct;
	initBuffer(68);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getDWord(0, &sourceId);
	getDWord(4, &targetId);
	getWord(8, &battleGroup);
	getWord(10, &itemMode);
	getArray(12, arState, 10);
	getArray(22, (BYTE*)arLife, 20);
	getArray(42, (BYTE*)arMana, 20);
	getWord(62, &sourceState);

	//sourceState = (sState == 100) ? true : false;
}


pItemAct::~pItemAct() {
	//
}


void pItemAct::process() {
	battle->handlePacket(this);
}

void pItemAct::debugPrint() {
	Packet::debugPrint();

	std::cout << "Battle Group: " << battleGroup << "Source>Target: " << sourceId << "(" << sourceState << ") > " << targetId << std::endl;
	std::cout << "ItemMode:" << itemMode << " Life/Mana:" << arLife[0] << "/" << arMana[0] << " EndingState:" << arState[0] << std::endl;

	std::cout << std::endl;
}