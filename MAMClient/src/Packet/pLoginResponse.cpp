#include "stdafx.h"
#include "Client.h"
#include "pLoginResponse.h"

#include "Window.h"

pLoginResponse::pLoginResponse(char* buf, char* encBuf) {
	description = "Login Response (Server)";
	type = ptLoginResponse;
	initBuffer(28);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getDWord(0, (DWORD*)&accountID);
	getDWord(4, (DWORD*)&seed);
	getString(8, (char*)phrase, 16);

	if (accountID == 0) {
		std::string phraseStr((char*)phrase);
		gClient.logPacketError(phraseStr);
	}
}


pLoginResponse::pLoginResponse(int acctID, int sd) {
	description = "Login Response (Client)";
	type = ptLoginResponse;
	initBuffer(28);	

	accountID = acctID;
	addDWord(0, (DWORD)acctID);

	seed = sd;
	addDWord(4, (DWORD)sd);

	memcpy(phrase, "blacknull", 9);
	addString(8, (char*)phrase, 16);
}


pLoginResponse::~pLoginResponse() {

}


void pLoginResponse::process() {
	gClient.handleLoginResponse(this);
}


void pLoginResponse::debugPrint() {
	Packet::debugPrint();

	std::cout << "Account ID: " << accountID << std::endl;
	std::cout << "Seed: " << seed << std::endl;
	std::cout << "Phrase: " << phrase << std::endl;

	std::cout << std::endl;
}