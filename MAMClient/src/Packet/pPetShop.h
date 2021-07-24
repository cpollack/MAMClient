#ifndef __PPETSHOP_H
#define __PPETSHOP_H
#include "Packet.h"

class pPetShop : public Packet {
public:
	int mode;

	pPetShop(int mode);
	~pPetShop();

	void pPetShop::debugPrint();
};

#endif