#include "stdafx.h"
#include "Client.h"
#include "pItemAction.h"

#include "Player.h"
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
	switch (action) {
	case iaUse:
		player->useItem(itemId);
		break;
	case iaBuy:
		player->buyItem(itemId);
		formMain->setCash(player->cash);
		break;
	case iaSell:
		player->sellItem(itemId);
		formMain->setCash(player->cash);
		break;
	case iaDrop:
		player->inventory->removeItem(itemId, true);
		break;
	}
}


void pItemAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Item ID: " << itemId << " Action: " << action << std::endl;

	std::cout << std::endl;
}