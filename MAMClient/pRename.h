#pragma once
#include "Packet.h"

enum RenameMode {
	rmPet = 0,
	rmNickname = 1,
	//rmName = 2, not yet confirme
	rmNpc = 6,
	rmItem = 7,
};


class pRename : public Packet {
public:
	int id;
	int mode;
	std::string name;

	pRename(char *buf, char* encBuf);
	pRename(int id, int mode, std::string name);
	~pRename();

	virtual void process();
	void pRename::debugPrint();
};
