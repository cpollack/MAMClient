#pragma once

#include "Packet.h"

/*enum NPCINFO_MODE {
	NPCINFO_MODE_NONE = 0,
	// 5
	// 10
	// ?
};*/

class pAiNpcInfo : public Packet {
public:
	typedef struct {
		DWORD	id;
		WORD	wType;
		WORD	wLook;
		WORD	wDir;
		WORD	wCellX;
		WORD	wCellY;
		WORD	wLevel;

		char szName[_MAX_NAMESIZE];
		BYTE bColorSets[15];
	}MSG_Info;

	MSG_Info* m_pInfo;

	pAiNpcInfo(char *buf, char* encBuf);
	~pAiNpcInfo();

	virtual void process();
	void pAiNpcInfo::debugPrint();
};