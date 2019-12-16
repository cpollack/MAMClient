#include "stdafx.h"
#include "Client.h"
#include "pBattleResponse.h"

#include "Battle.h"

pBattleResponse::pBattleResponse(char* buf, char* encBuf) {
	description = "Battle Response (Server)";
	type = ptBattleResponse;
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

	getInt(4, &sourceId);
	getInt(8, &targetId);
	getWord(12, &damage);
	getWord(14, &v1);
	getWord(16, &v2);
	getWord(18, &v3);
}


pBattleResponse::~pBattleResponse() {

}


void pBattleResponse::process() {
	battle->handlePacket(this);
}

void pBattleResponse::debugPrint() {
	Packet::debugPrint();

	std::cout << "Action: " << action << " Group: " << group << "Source>Target: " << sourceId << "(" << sourceState << ") > " << targetId << "(" << targetState << ") Damage: " << damage << std::endl;
	std::cout << "Unknown Values: " << v1 << " / " << v2 << " / " << v3 << std::endl;

	std::cout << std::endl;
}