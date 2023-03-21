#include "stdafx.h"
#include "Client.h"
#include "pAiNpcInfo.h"

#include "GameMap.h"


pAiNpcInfo::pAiNpcInfo(char *buf, char* encBuf) {
	description = "NPC Info (Client)";
	type = ptAiNpcInfo;
	initBuffer(sizeof(MSG_Info) + 4);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	m_pInfo = new MSG_Info;
	memcpy(m_pInfo, buf+4, size);
	int a = 1;
	/*getInt(0, &id);
	getWord(4, &mode);
	getWord(6, &npcType);
	getWord(8, &look);
	getWord(10, &posX);
	getWord(12, &posY);

	getString(14, name, 16);
	getString(30, (char*)hslSets, 15);*/
}


pAiNpcInfo::~pAiNpcInfo() {
	delete m_pInfo;
}


void pAiNpcInfo::process() {
	map->addNpc(this);
}


void pAiNpcInfo::debugPrint() {
	Packet::debugPrint();

	std::cout << "AiNPC ID: " << m_pInfo->id << " Type: " << m_pInfo->wType << " Look: " << m_pInfo->wLook << " Dir: " << m_pInfo->wDir << " Position: " << m_pInfo->wCellX << "," << m_pInfo->wCellY << std::endl;
	//std::cout << "AiNPC ID: " << id << " Type: " << type << " Look: " << look << " Position: " << posX << "," << posY << std::endl;
	//std::cout << name << " - Mode: " << mode << " Vals: " << hslSets << std::endl;

	std::cout << std::endl;
}