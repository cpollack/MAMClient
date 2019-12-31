#include "stdafx.h"
#include "Client.h"
#include "pRename.h"

#include "Player.h"
#include "CustomEvents.h"


pRename::pRename(char *buf, char* encBuf) {
	description = "Action (Server)";
	type = ptRename;
	initBuffer(28);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &id);
	getByte(4, &mode);

	char buffer[16];
	getString(5, buffer, 16);
	name = buffer;
}


pRename::pRename(int id, int mode, std::string name) {
	description = "Action (Client)";
	type = ptRename;
	initBuffer(28);

	this->id = id;
	this->mode = mode;
	this->name = name;

	addDWord(0, id);
	addByte(4, mode);
	addString(5, (char*)name.c_str(), 16);
}


pRename::~pRename() {
	//
}


void pRename::process() {
	switch (mode) {
	case rmNickname:
		player->SetNickName(name);

		//Alerts player status screen of rename
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_PLAYER;
		event.user.code = PLAYER_RENAME;
		SDL_PushEvent(&event);
	}
}


void pRename::debugPrint() {
	Packet::debugPrint();

	std::string modeText;
	switch (mode) {
	case rmPet: modeText = "Pet"; break;
	case rmNickname: modeText = "Nickname"; break;
	case rmNpc: modeText = "NPC"; break;
	case rmItem: modeText = "Item"; break;
	default: modeText = "Unknown";
	}
	std::cout << "Mode: " << mode << " - " << modeText << std::endl;
	std::cout << "ID: " << id << " Name: " << name << std::endl;

	std::cout << std::endl;
}