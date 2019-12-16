#include "stdafx.h"
#include "Client.h"
#include "pNpc.h"


pNpc::pNpc(int npcId, int npcType, int val3, int val4) {
	description = "NPC (Client)";
	type = ptNpc;
	initBuffer(16);

	id = npcId;
	this->npcType = npcType;

	addDWord(0, id);
	addWord(4, npcType);
	addWord(6, val3);
	addDWord(8, val4);
}


pNpc::~pNpc() {

}


void pNpc::debugPrint() {
	Packet::debugPrint();

	std::cout << "NPC ID: " << id << " Type: " << type << std::endl;

	std::cout << std::endl;
}