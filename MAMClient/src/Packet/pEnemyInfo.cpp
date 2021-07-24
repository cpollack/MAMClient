#include "stdafx.h"
#include "Client.h"
#include "pEnemyInfo.h"

#include "Battle.h"

pEnemyInfo::pEnemyInfo(int pSize, char* buf, char* encBuf) {
	description = "Battle - Enemy Info (Server)";
	type = ptEnemyInfo;
	initBuffer(pSize);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &enemyCount);

	enemy = new EnemyInfo[enemyCount];
	for (int i = 0; i < enemyCount; i++) {
		int offset = 24 * i;
		getInt(4 + offset, &enemy[i].monsterId);
		getWord(8 + offset, &enemy[i].look);
		getWord(10 + offset, &enemy[i].level);
		getString(12 + offset, enemy[i].name, 16);
	}
}


pEnemyInfo::~pEnemyInfo() {
	if (enemy) delete[] enemy;
}


void pEnemyInfo::process() {
	battle->handlePacket(this);
}

void pEnemyInfo::debugPrint() {
	Packet::debugPrint();

	std::cout << "Enemy Count: " << enemyCount << std::endl;
	for (int i = 0; i < enemyCount; i++) {
		std::cout << enemy[i].monsterId << " " << enemy[i].name << " -  Level " << enemy[i].level << " Look:" << enemy[i].look << std::endl;
	}

	std::cout << std::endl;
}