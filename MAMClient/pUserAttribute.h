#pragma once

#include "Packet.h"

enum AttributeUpdate {
	UPD_LIFECURRENT,
	UPD_LIFEMAX,
	UPD_MANACURRENT,
	UPD_MANAMAX,
	UPD_MONEY,
	UPD_EXP,
	UPD_REPUTATION,
	UPD_THIEVERY,
	UPD_KUNGFU,
	UPD_PETRAISING,
	UPD_WUXING,
	UPD_DEPOSITED,
	UPD_ATTRPOINT,
	UPD_LOOK,
	UPD_LEVEL,
	UPD_PHYSIQUE,
	UPD_STAMINA,
	UPD_FORCE,
	UPD_SPEED,
	UPD_DEGREE,
	UPD_PK,
	UPD_EMOTION,
	UPD_CULTIVATION,
	UPD_LOCKKEY,
	UPD_ALIGNMENT,
	UPD_RANK,
	UPD_VIRTUE,
	UPD_REBORN
};

struct AttributeChange {
	int code;
	int value;
};

class pUserAttribute : public Packet {
public:
	int userId;
	int count;
	std::vector<AttributeChange> changes;

	pUserAttribute(int size, char *buf, char* encBuf);
	~pUserAttribute();

	virtual void process();
	void pUserAttribute::debugPrint();
};