#include "stdafx.h"
#include "Client.h"
#include "pGsConnect.h"

#include "MainWindow.h"
#include "LoginForm.h"
#include "Player.h"

pGsConnect::pGsConnect(char* buf, char* encBuf) {
	description = "GameServer Connect (Server)";
	type = ptGsConnect;
	initBuffer(32);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getDWord(0, (DWORD*)&accountId);
	getDWord(4, (DWORD*)&seed);

	getWord(8, (WORD*)&responseCode);

	getString(10, (char*)response, 16);
}


pGsConnect::pGsConnect(int acctId, int sd, int respCode, char* resp) {
	description = "GameServer Connect (Client)";
	type = ptGsConnect;
	initBuffer(32);

	accountId = acctId;
	seed = sd;
	responseCode = respCode;
	memcpy(response, resp, strlen(resp));

	addDWord(0, accountId);
	addDWord(4, seed);

	addWord(8, responseCode);

	addString(10, (char*)response, 16);
}


pGsConnect::~pGsConnect() {

}


void pGsConnect::process() {
	if (player) { // Character exists, Send connection response
		pGsConnect* connectResponse = new pGsConnect(accountId, 0, 4010, (char*)player->getName().c_str());
		gClient.addPacket(connectResponse);
	}
	else {
		gClient.mainReady = true;
	}
}


void pGsConnect::debugPrint() {
	Packet::debugPrint();

	std::cout << "Account ID: " << accountId << ", Seed: " << seed << std::endl;
	std::cout << "Response (" << responseCode << "): " << response << std::endl;

	std::cout << std::endl;
}