#include "stdafx.h"
#include "pFriend.h"

pFriend::pFriend(int pSize, char* buf, char* encBuf) {
	description = "Friend (Server)";
	type = ptFriend;
	initBuffer(pSize);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getByte(0, &mode);
	getByte(1, &count);

	int pos = 4;
	for (int i = 0; i < count; i++) {
		getInt(pos, &friends[i].userId);
		pos += 4;
		getInt(pos, (int*)&friends[i].online);
		pos += 4;
	}

	std::vector<std::string> friendNames = getStringPack(164);
	for (int i = 0; i < count; i++) {
		if (friendNames.size() > i) friends[i].userName = friendNames[i];
	}
}


pFriend::~pFriend() {
	//
}


void pFriend::process() {
	//
}

void pFriend::debugPrint() {
	Packet::debugPrint();

	std::cout << "Mode: " << mode << std::endl;

	std::cout << std::endl;
}