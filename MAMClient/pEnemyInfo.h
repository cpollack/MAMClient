#ifndef __PENEMYINFO_H
#define __PENEMYINFO_H
#include "Packet.h"

struct EnemyInfo {
	int monsterId = 0;
	int look = 0;
	int level = 0;
	char name[16] = { 0 };
};

class pEnemyInfo : public Packet {
public:
	int enemyCount = 0;
	EnemyInfo* enemy;

	pEnemyInfo(int pSize, char *buf, char* encBuf);
	~pEnemyInfo();

	virtual void process();
	void pEnemyInfo::debugPrint();
};

#endif