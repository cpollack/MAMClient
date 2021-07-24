#include "stdafx.h"
#include "Client.h"
#include "pNpcDialogue.h"

#include "GameMap.h"


pNpcDialogue::pNpcDialogue(int size, char *buf, char* encBuf) {
	description = "NPC Dialogue (Server)";
	type = ptNpcDialogue;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getByte(0, &unk1); //These come from the player, currently unknown
	getByte(1, &unk2);
	getByte(2, &unk3);
	getByte(3, &unk4);
	getWord(4, &npcFace);
	getWord(6, &mode);

	//strings
	int iterCount = 0;
	getByte(8, &iterCount);
	int nextPos = 9;
	for (int i = 0; i < iterCount; i++) {
		int nextSize = 0;
		getByte(nextPos++, &nextSize);

		if (nextSize > 0) {
			char nextStr[256] = { 0 };
			getString(nextPos, nextStr, nextSize);
			nextPos += nextSize;

			if (nextSize > 0) {
				// Stringpacks 0-3 are the responses, 4+ are the messages
				if (i < 4) responses.push_back(nextStr);
				else messages.push_back(nextStr);
			}
		}
	}
}


pNpcDialogue::~pNpcDialogue() {

}


void pNpcDialogue::process() {
	map->createDialogue(this);
}


void pNpcDialogue::debugPrint() {
	Packet::debugPrint();

	std::cout << "Face: " << npcFace << " Mode: " << mode << std::endl; 
	std::cout << " Unknown Vals: " << unk1 << "," << unk2 << "," << unk3 << "," << unk4 << std::endl;

	for (auto msg : messages) {
		std::cout << msg << std::endl;
	}
	for (auto msg : responses) {
		std::cout << msg << std::endl;
	}

	std::cout << std::endl;
}