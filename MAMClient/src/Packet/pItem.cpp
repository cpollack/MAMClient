#include "stdafx.h"
#include "Client.h"
#include "pItem.h"

#include "Player.h"
#include "Pet.h"
#include "Item.h"
#include "CustomEvents.h"

pItem::pItem(char* buf, char* encBuf) {
	description = "Item (Server)";
	type = ptItem;
	size = 78;
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

	getWord(66, &antiPoison);
	getWord(68, &antiFreeze);
	getWord(70, &antiSleep);
	getWord(72, &antiChaos);

	getWord(74, &count);
	getWord(76, &stack);
}


pItem::~pItem() {
	//
}


void pItem::process() {
	Item *item;
	SDL_Event e;

	std::vector<Pet*> petList;

	switch (mode) {
	case imInventory:
		item = new Item(this);
		player->addItem(item);

		SDL_zero(e);
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_ADD;
		e.user.data1 = item;
		SDL_PushEvent(&e);
		break;
	case imEquipment:
		player->setEquipment(this);
		break;
	case imPetAccessory:
		item = new Item(this);

		petList = player->getPetList();
		for (auto pet : petList) {
			if (pet->GetID() == ownerId) {
				pet->setItem(item);
				break;
			}
		}
		break;
	case imWuxingPreview:
		item = new Item(this);

		// Send item event preview to the wuxing window
		SDL_zero(e);
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_PREVIEW;
		e.user.data1 = item;
		SDL_PushEvent(&e);
		break;
	}
}


void pItem::debugPrint() {
	hideEncryptedBuffer = true;
	Packet::debugPrint();

	std::cout << "[" << mode << "]" << name << " (" << itemId << ") " << creator << " Owner: " << ownerId << std::endl;
	std::cout << "Cost: " << cost << " Look: " << look << " Sort: " << itemSort << std::endl;
	std::cout << "Level: " << levelRequired << " HP/Mana: " << life << "/" << power << " At/Df/Dx: " << attack << "/" << defence << "/" << dexterity << std::endl;
	std::cout << "Anti Poison/Freeze/Sleep/Chaos: " << antiPoison << "/" << antiFreeze << "/" << antiSleep << "/" << antiChaos << std::endl;
	std::cout << "Count: " << count << "/" << stack << std::endl;

	std::cout << std::endl;
}