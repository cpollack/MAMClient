#include "stdafx.h"
#include "Client.h"
#include "pBattleRound.h"

#include "Battle.h"

pBattleRound::pBattleRound(char* buf, char* encBuf) {
	description = "Battle Round (Server)";
	type = ptBattleRound;
	initBuffer(20);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	count = 0;
	for (int i = 0; i < 20; i++) {
		int sState = 0;
		getByte(i, &sState);
		if (sState > 0) {
			alive[i] = (sState == 100) ? true : false;
			count++;
		}
	}
}


pBattleRound::~pBattleRound() {

}


void pBattleRound::process() {
	battle->handlePacket(this);
}

void pBattleRound::debugPrint() {
	Packet::debugPrint();

	for (int i = 0; i < count; i++) {
		std::cout << "Fighter" << i << ":" << ((alive[i]) ? "alive " : "dead ");
	}	
	std::cout << std::endl;

	std::cout << std::endl;
}