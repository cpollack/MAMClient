#include "stdafx.h"
#include "Client.h"
#include "pWuxing.h"

pWuxing::pWuxing(char* buf, char* encBuf) {
	description = "Wuxing (Server)";
	type = ptWuxing;
	initBuffer(72);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	//currently never received
}


pWuxing::pWuxing(int iMode, int iMain, std::vector<PACKET_WUX_SLOT> vSlots) {
	description = "Wuxing (Client)";
	type = ptWuxing;
	initBuffer(72);

	mode = iMode;
	addWord(0, mode);
	mainIngredient = iMain;
	addWord(2, mainIngredient);

	int next = 4;
	for (int i = 0; i < 8; i++) {
		if (i < vSlots.size()) {
			slots[i].type = vSlots[i].type;
			slots[i].id = vSlots[i].id;
		}
		else {
			slots[i].type = 0;
			slots[i].id = 0;
		}
		addDWord(next, slots[i].type);
		next += 4;
		addDWord(next, slots[i].id);
		next += 4;
	}
}


pWuxing::~pWuxing() {

}


void pWuxing::process() {
	//
}


void pWuxing::debugPrint() {
	Packet::debugPrint();

	std::cout << "Mode " << mode << " main ingredient " << mainIngredient << std::endl;

	std::cout << std::endl;
}