#include "stdafx.h"
#include "Client.h"
#include "pItemAction.h"
#include "CustomEvents.h"

#include "Player.h"
#include "Inventory.h"
#include "FormMain.h"


pItemAction::pItemAction(char *buf, char* encBuf) {
	description = "Item Action (Server)";
	type = ptItemAction;
	initBuffer(12);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &itemId);
	getInt(4, &action);
}


pItemAction::pItemAction(int id, int act) {
	description = "Item Action (Client)";
	type = ptItemAction;
	initBuffer(12);

	itemId = id;
	action = act;

	addDWord(0, id);
	addDWord(4, act);
}


pItemAction::~pItemAction() {

}


void pItemAction::process() {
	SDL_Event e;
	SDL_zero(e);
	switch (action) {
	case iaUse:
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_USE;
		e.user.data1 = player->inventory->getItem(itemId);
		SDL_PushEvent(&e);

		player->useItem(itemId);
		break;
	case iaBuy:
		player->buyItem(itemId);
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_BUY;
		e.user.data1 = player->inventory->getItem(itemId);
		SDL_PushEvent(&e);

		e.type = CUSTOMEVENT_PLAYER;
		e.user.code = PLAYER_MONEY;
		SDL_PushEvent(&e);
		break;
	case iaSell:
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_SELL;
		e.user.data1 = player->inventory->getItem(itemId);
		SDL_PushEvent(&e);
		player->sellItem(itemId);

		e.type = CUSTOMEVENT_PLAYER;
		e.user.code = PLAYER_MONEY;
		SDL_PushEvent(&e);
		break;
	case iaDrop:
		e.type = CUSTOMEVENT_ITEM;
		e.user.code = ITEM_DROP;
		e.user.data1 = player->inventory->getItem(itemId);
		SDL_PushEvent(&e);

		player->inventory->removeItem(itemId, true);
		break;
	}
}


void pItemAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Item ID: " << itemId << " Action: " << action << std::endl;

	std::cout << std::endl;
}