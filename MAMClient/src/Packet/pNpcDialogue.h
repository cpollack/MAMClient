#ifndef __PNPCDIALOGUE_H
#define __PNPCDIALOGUE_H
#include "Packet.h"

class pNpcDialogue : public Packet {
public:
	int npcFace, mode;
	int stringCount;
	std::vector<std::string> messages;
	std::vector<std::string> responses;

	int unk1, unk2, unk3, unk4;

	pNpcDialogue(int size, char *buf, char* encBuf);
	~pNpcDialogue();

	virtual void process();
	void pNpcDialogue::debugPrint();
};

#endif