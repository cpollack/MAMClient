#include "stdafx.h"
#include "Client.h"
#include "pPetShop.h"


pPetShop::pPetShop(int mode) {
	description = "Pet Shop (Client)";
	type = ptPetShop;
	initBuffer(20);

	this->mode = mode;
	addWord(12, mode);
}


pPetShop::~pPetShop() {

}


void pPetShop::debugPrint() {
	Packet::debugPrint();

	std::cout << "Mode: " << mode << std::endl;

	std::cout << std::endl;
}