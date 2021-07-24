#include "stdafx.h"
#include "pEmotion.h"

pEmotion::pEmotion(char* buf, char* encBuf) {
	description = "Emotion (Server)";
	type = ptEmotion;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &sourceId);
	getWord(4, &x);
	getWord(6, &y);
	getWord(8, &emotion);
}


pEmotion::~pEmotion() {
	//
}


void pEmotion::process() {
	//Pass to user manager for updating player emotions
}


void pEmotion::debugPrint() {
	Packet::debugPrint();

	std::cout << "Source Id: " << sourceId << std::endl;
	std::cout << "Position: " << x << ", " << y << " Emotion: " << emotion << std::endl;

	std::cout << std::endl;
}