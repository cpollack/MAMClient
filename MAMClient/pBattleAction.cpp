#include "stdafx.h"
#include "Client.h"
#include "pBattleAction.h"

#include "Battle.h"

pBattleAction::pBattleAction(char* buf, char* encBuf) {
	description = "Battle Action (Server)";
	type = ptBattleAction;
	initBuffer(24);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getByte(0, &action);
	getByte(1, &round);
	getInt(4, &sourceId);
	getInt(8, &targetId);
	getInt(12, &useId);
	getInt(16, &verification);
}


pBattleAction::pBattleAction(int act, int rnd, int srcId, int trgId, int usId, int accountId) {
	description = "Battle Action (Client)";
	type = ptBattleAction;
	initBuffer(24);

	action = act;
	round = rnd;
	sourceId = srcId;
	targetId = trgId;
	useId = usId;

	addByte(0, action);
	addByte(1, round);
	addDWord(4, sourceId);
	addDWord(8, targetId);
	addDWord(12, useId);

	verification = (accountId ^ sourceId) + sourceId * (action + 8210 - round);
	addDWord(16, verification);
}


pBattleAction::~pBattleAction() {

}


void pBattleAction::process() {
	battle->handlePacket(this);
}

void pBattleAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Action: " << action << " Round: " << round << "Source>Target: " << sourceId << " > " << targetId << " Using: " << useId << " - Verification: " << verification << std::endl;

	std::cout << std::endl;
}