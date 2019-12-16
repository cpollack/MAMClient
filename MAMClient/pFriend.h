#pragma once

#include "Packet.h"

struct pFriendEntry {
	int userId = 0;
	bool online = false;
	std::string userName;
};

enum pFriendMode {
	pfmRequest = 10, //friend request
	pfmAdd = 11, //add friend
	//12 - send friend info?
	pfmSpouse = 13, //husband / wife info? or online/offline
	pfmFullInfo = 15 //15 - friend info
};

class pFriend : public Packet {
public:
	int mode;
	int count;
	pFriendEntry friends[20];

	pFriend(int pSize, char *buf, char* encBuf);
	~pFriend();

	virtual void process();
	void debugPrint();
};
