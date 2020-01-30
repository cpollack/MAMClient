#include "stdafx.h"
#include "Client.h"
#include "pNormalAct.h"

#include "Battle.h"

pNormalAct::pNormalAct(char* buf, char* encBuf) {
	description = "Battle - Normal Act (Server)";
	type = ptNormalAct;
	initBuffer(24);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getByte(0, &action);
	getByte(1, &group);

	int sState = 0, tState = 0;
	getByte(2, &sState);
	getByte(3, &tState);
	sourceState = (sState == 100) ? true : false;
	targetState = (tState == 100) ? true : false;

	getDWord(4, &sourceId);
	getDWord(8, &targetId);
	getWord(12, &damage);
	getWord(14, &v1);
	getWord(16, &v2);
	getWord(18, &v3);
}


pNormalAct::~pNormalAct() {

}


void pNormalAct::process() {
	battle->handlePacket(this);
}

void pNormalAct::debugPrint() {
	Packet::debugPrint();

	std::cout << "Action: " << action << " Group: " << group << "Source>Target: " << sourceId << "(" << sourceState << ") > " << targetId << "(" << targetState << ") Damage: " << damage << std::endl;
	std::cout << "Unknown Values: " << v1 << " / " << v2 << " / " << v3 << std::endl;

	std::cout << std::endl;
}