#include "stdafx.h"
#include "Client.h"
#include "pRole.h"

#include "Player.h"

pRole::pRole(char* buf, char* encBuf) {
	description = "Role (Server)";
	type = ptRole;
	initBuffer(80);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getString(0, (char*)nickName, 16);
	getString(16, (char*)name, 16);

	getWord(32, (WORD*)&point_life);
	getWord(34, (WORD*)&point_def);
	getWord(36, (WORD*)&point_attack);
	getWord(38, (WORD*)&point_dex);
	getWord(40, (WORD*)&point_mana);

	getByte(42, (BYTE*)&look);
	getByte(43, (BYTE*)&face);

	getDWord(44, (DWORD*)&mapId);

	getString(48, (char*)hsl, 25);
}


pRole::pRole(int aLook, int aFace, int aMapId, int life, int mana, int attack, int def, int dex, char* aName, char* aNickName, char* hslSets) {
	description = "Role (Client)";
	type = ptRole;
	initBuffer(80);

	look = aLook;
	face = aFace;
	mapId = aMapId;
	point_life = life;
	point_mana = mana;
	point_attack = attack;
	point_def = def;
	point_dex = dex;
	memcpy(name, aName, strlen(aName));
	memcpy(nickName, aNickName, strlen(aNickName));
	memcpy(hsl, hslSets, 25);
	
	addString(0, (char*)nickName, 16);
	addString(16, (char*)name, 16);

	addWord(32, point_life);
	addWord(34, point_def);
	addWord(36, point_attack);
	addWord(38, point_dex);
	addWord(40, point_mana);

	addByte(42, look);
	addByte(43, face);

	addDWord(44, mapId);

	addString(48, (char*)hsl, 25);
}


pRole::~pRole() {

}


void pRole::process() {
	player = new Player(gClient.accountId, look, face, (char*)name);
}


void pRole::debugPrint() {
	Packet::debugPrint();

	std::cout << "Name: " << name << ", Nickname: " << nickName << std::endl;
	std::cout << "Look and Face: " << look << " " << face << std::endl;
	std::cout << "Map ID: " << mapId << std::endl;
	std::cout << "Stats: " << point_life << ", " << point_mana << ", " << point_attack << ", " << point_def << ", " << point_dex << std::endl;

	std::cout << std::endl;
}