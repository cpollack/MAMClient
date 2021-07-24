#include "stdafx.h"
#include "pLoginRequest.h"
#include "RC5.h"

pLoginRequest::pLoginRequest(char* userName, char* password, char* server) {
	user = userName;
	pass = password;
	serv = server;

	description = "Login Request";
	type = ptLoginRequest;
	initBuffer(52);

	addString(0, userName, strlen(userName));

	//char encryptedPassword[16];
	//EncryptPassword ep;
	//ep.encrypt(encryptedPassword, password);

	unsigned char key1[16] = { 0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E, 0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE };
	//unsigned char key2[16] = { 0x44, 0xD0, 0xE2, 0xBA, 0x4A, 0x38, 0x14, 0x44, 0x64, 0xE0, 0x12, 0xAE, 0xDA, 0x56, 0x1C, 0xF8 };

	RC5 rc1;
	rc1.Rc5InitKey(key1);

	unsigned char encryptedPassword[16] = { 0 };
	memcpy(encryptedPassword, pass.c_str(), pass.length());
	rc1.Rc5Encrypt(encryptedPassword, 16);

	addString(16, (char*)encryptedPassword, 16);

	addString(32, server, strlen(server));
}


pLoginRequest::~pLoginRequest() {

}


void pLoginRequest::debugPrint() {
	Packet::debugPrint();
	
	std::cout << "User: " << user << std::endl;
	std::cout << "Password: " << pass << std::endl;
	std::cout << "Server: " << serv << std::endl;

	std::cout << std::endl;
}