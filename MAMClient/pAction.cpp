#include "stdafx.h"
#include "pAction.h"

pAction::pAction(char* buf, char* encBuf) {
	description = "Action (Server)";
	type = ptAction;
	initBuffer(16);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &sourceId);
	getWord(4, &value1);
	getWord(6, &value2);
	getWord(8, &value3);
	getWord(10, &value4);
}


pAction::~pAction() {
	//
}


void pAction::process() {
	//
}


void pAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Source Id: " << sourceId << std::endl;
	std::cout << "Value1: " << value1 << " Value2: " << value2 << " Value3: " << value3 << " Value4: " << value4 << std::endl;

	std::cout << std::endl;
}