#include "stdafx.h"
#include "Client.h"
#include "pTick.h"

pTick::pTick(char* buf, char* encBuf) {
	description = "Tick (Server)";
	type = ptTick;
	initBuffer(12);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getDWord(4, &systemTime);
}


pTick::pTick(int uId) {
	description = "Tick (Client)";
	type = ptTick;
	initBuffer(12);

	userId = uId;
	addDWord(0, userId);

	systemTime = timeGetTime();
	DWORD encryptedTime = systemTime ^ (userId * userId + 9527);
	addDWord(4, encryptedTime);
}


pTick::~pTick() {

}


void pTick::process() {
	pTick* responseTick = new pTick(userId);
	gClient.addPacket(responseTick);
}


void pTick::debugPrint() {
	return;
	Packet::debugPrint();

	std::cout << "User Id: " << userId << " Timestamp: " << systemTime << std::endl;

	std::cout << std::endl;
}