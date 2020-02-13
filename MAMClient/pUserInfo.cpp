#include "stdafx.h"
#include "pUserInfo.h"

#include "UserManager.h"
#include "User.h"

pUserInfo::pUserInfo(int pSize, char* buf, char* encBuf) {
	description = "User Info (Server)";
	type = ptUserInfo;
	initBuffer(pSize);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getWord(4, &x);
	getWord(6, &y);
	getByte(8, &look);
	getByte(9, &face);
	getByte(10, &state);
	getByte(11, &direction);
	getByte(12, &emotion);
	getByte(13, &rank);
	getByte(14, &reborns);
	getByte(15, &rankType);
	getWord(16, &level);
	getInt(20, &pkEnabled);
	getInt(24, &syndicateId);
	getInt(28, &subGroudId);
	getWord(32, &syndicateRank);
	getString(34, (char*)colorSets, 25);

	std::vector<std::string> strings = getStringPack(59);
	if (strings.size() > 0) name = strings[0];
	if (strings.size() > 1) nickName = strings[1];
	if (strings.size() > 2) spouse = strings[2];
	int nextStr = 3;
	if (syndicateId > 0 && strings.size() > nextStr) guild = strings[(nextStr++ - 1)];
	if (subGroudId > 0 && strings.size() > nextStr) subGroup = strings[(nextStr++ - 1)];
	if (syndicateId > 0 && strings.size() > nextStr) guildTitle = strings[(nextStr++ - 1)];

	masterType = rankType / 16;
	alignment = rankType % 16;
}


pUserInfo::~pUserInfo() {
	//
}


void pUserInfo::process() {
	User* user = new User(this);
	user->loadSprite();
	userManager.addUser(user);
}

void pUserInfo::debugPrint() {
	Packet::debugPrint();

	std::cout << "@" << x << "," << y << "(D"<<direction<<") " << name << "(" << nickName << " <3:" << spouse << ") Look: " << look << " Emotion: " << emotion << std::endl;
	std::cout << "Level: " << level << " Rank: " << rank << " Master: " << masterType << " Alignemnt: " << alignment << " Reborns:" << reborns << std::endl;
	if (syndicateId) {
		std::cout << guild << "(" << syndicateId << ") ";
		if (subGroudId) std::cout << subGroup << "(" << subGroudId << ") ";
		std::cout << "Rank: " << rank  << "(" << rankType << ")" << std::endl;
	}
	std::cout << "Face:" << face << " State? " << state << std::endl;
	std::cout << std::endl;
}
