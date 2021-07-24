#pragma once

#include "Packet.h"

class pMessage : public Packet {
public:
	int color;
	int channel;
	int effect;
	int time;
	char emotion[16];

	std::string sender;
	std::string target;
	std::string message;

	int hour, min;

	pMessage(int size, char *buf, char* encBuf);
	pMessage(int channel, std::string sender, std::string target, std::string message, int color, int effect, std::string emotion);
	~pMessage();

	virtual void process();
	void pMessage::debugPrint();
};