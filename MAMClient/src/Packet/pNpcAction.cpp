#include "stdafx.h"
#include "Client.h"
#include "pNpcAction.h"


pNpcAction::pNpcAction(int option, int mode, int val1) {
	description = "NPC Action (Client)";
	type = ptNpcAction;
	initBuffer(12);

	this->option = option;
	this->mode = mode;

	addDWord(0, option);
	addDWord(4, mode);
}


pNpcAction::~pNpcAction() {

}


void pNpcAction::debugPrint() {
	Packet::debugPrint();

	std::cout << "Option: " << option << " Mode: " << mode << std::endl;

	std::cout << std::endl;
}