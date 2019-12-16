#include "stdafx.h"
#include "Client.h"
#include "pItem.h"

#include "Player.h"

pItem::pItem(char* buf, char* encBuf) {
	description = "Item (Server)";
	type = ptItem;
	size = 96;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &mode);
	getInt(4, &ownerId);
	getString(8, name, 16);
	getString(24, creator, 16);

	getInt(40, &itemId);
	getInt(44, &cost);
	getWord(48, &look);
	getWord(50, &itemSort);

	getInt(52, &levelRequired);
	getWord(56, &life);
	getWord(58, &power);
	getWord(60, &attack);
	getWord(62, &defence);
	getWord(64, &dexterity);

	getWord(66, &v1);
	getWord(68, &v2);
	getWord(70, &v3);
	getWord(72, &v4);

	getInt(74, &v5);
	getInt(78, &v6);
	getInt(82, &v7);
	getInt(86, &v8);
}


pItem::~pItem() {
	//
}


void pItem::process() {
	switch (mode) {
	case imInventory:
		player->addItem(this);
		break;
	case imEquipment:
		player->setEquipment(this);
		break;
	}
}


void pItem::debugPrint() {
	hideEncryptedBuffer = true;
	Packet::debugPrint();

	std::cout << "[" << mode << "]" << name << " (" << itemId << ") " << creator << " Owner: " << ownerId << std::endl;
	std::cout << "Cost: " << cost << " Look: " << look << " Sort: " << itemSort << std::endl;
	std::cout << "Level: " << levelRequired << " HP/Mana: " << life << "/" << power << " At/Df/Dx: " << attack << "/" << defence << "/" << dexterity << std::endl;
	std::cout << "V1: " << v1 << " V2: " << v2 << " V3: " << v3 << " V4: " << v4 << " V5: " << v5 << " V6: " << v6 << " V7: " << v7 << " V8: " << v8 << std::endl;

	std::cout << std::endl;
}