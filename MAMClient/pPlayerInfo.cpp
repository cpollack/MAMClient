#include "stdafx.h"
#include "Client.h"
#include "pPlayerInfo.h"

#include "MainWindow.h"
#include "Player.h"

pPlayerInfo::pPlayerInfo(int size, char* buf, char* encBuf) {
	description = "Player Info (Server)";
	type = ptPlayerInfo;
	this->size = size;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);

	getByte(4, &look);
	getByte(5, &face);

	getWord(6, &level);
	getWord(8, &life_current);
	getWord(10, &life_max);
	getWord(12, &mana_current);
	getWord(14, &mana_max);

	getByte(16, &rank);
	getByte(17, &reborns);
	BYTE rankInfo;
	getByte(18, &rankInfo);
	masterType = (rankInfo & 0xF0) >> 4;
	alignment = (rankInfo & 0x0F);

	getInt(20, &cash);
	getInt(24, &reputation);
	int pkVal;
	getInt(28, &pkVal);
	pkEnabled = (pkVal < 0) ? false : true;
	getInt(32, &cultivation);
	getInt(36, &experience);
	getInt(40, &wuxing);
	getInt(44, &kungfu);
	getInt(48, &petRaising);
	getInt(52, &thievery);

	getWord(56, &unused_point);
	getWord(58, &life_point);
	getWord(60, &defence_point);
	getWord(62, &attack_point);
	getWord(64, &dexterity_point);
	getWord(66, &mana_point);

	getWord(68, &mapX);
	getWord(70, &mapY);
	getInt(72, &mapId);

	getInt(76, &syndicateId);
	getInt(80, &syndicateSubId);
	getWord(84, &syndicateRank);

	int pos = 86;
	int stringCount = 0;
	std::vector<char*> strings;
	getByte(pos++, &stringCount);
	if (stringCount > 0) {
		for (int i = 0; i < stringCount; i++) {
			int strSize = 0;
			getByte(pos++, &strSize);
			if (strSize > 0) {
				char* newStr = new char[strSize+1]{ 0 };
				getString(pos, newStr, strSize);
				strings.push_back(newStr);
				pos += strSize;
			}
			else strings.push_back(nullptr);
		}
	}
	if (strings.size() < 6) {
		for (int i = 0; i <= 6 - strings.size(); i++)
			strings.push_back(nullptr);
	}
	name = (BYTE*)strings[0];
	nickName = (BYTE*)strings[1];
	spouse = (BYTE*)strings[2];
	syndicate = (BYTE*)strings[3];
	branch = (BYTE*)strings[4];
	position = (BYTE*)strings[5];
}


pPlayerInfo::pPlayerInfo() {
	//
}


pPlayerInfo::~pPlayerInfo() {
	//
}


void pPlayerInfo::process() {
	player->setPlayerInfo(this);
	mainForm->setPlayerDetailsLabels();
}


void pPlayerInfo::debugPrint() {
	hideEncryptedBuffer = true;
	Packet::debugPrint();

	int fullRank = (masterType * 10000) + (rank * 1000) + (reborns * 10) + alignment;
	std::cout << (char*)name << " " << (char*)nickName << " Spouse: " << (((char*)spouse) ? (char*)spouse : "") << std::endl;
	std::cout << "User ID: " << userId << ", Look/Face: " << look <<  "/" << face <<  " Rank: " << fullRank << std::endl;
	std::cout << "Level: " << level << " Exp: " << experience << " Life:" << life_current << "/" << life_max << "  Mana:" << mana_current << "/" << mana_max << std::endl;
	std::cout << "Cash: " << cash << " Reputation: " << reputation << " Cultivation: " << cultivation << " " << "Martial Circle: " << ((pkEnabled) ? "Yes" : "No") << std::endl;
	std::cout << "Wuxing: " << wuxing << " Kungfu: " << kungfu << " Pet Raising: " << petRaising << " Thievery: " << thievery << std::endl;
	std::cout << "Health/Mana: " << life_point << "/" << mana_point << " Attack/Def/Dex:" << attack_point << "/" << defence_point << "/" << dexterity_point << " (Unused " << unused_point << ")" << std::endl;
	std::cout << "Map: " << mapId << " " << mapX << "," << mapY << std::endl;
	if (syndicate) {
		std::cout << (char*)syndicate;
		if (branch) std::cout << " " << (char*)branch;
		std::cout << " " << (char*)position;
		std::cout << std::endl;
		std::cout << "Syndicate Id: " << syndicateId << " Sub Id: " << syndicateSubId << " Rank: " << syndicateRank << std::endl;
	}

	std::cout << std::endl;
}