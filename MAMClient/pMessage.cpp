#include "stdafx.h"
#include "Client.h"
#include "pMessage.h"

#include <ctime>

#include "MessageManager.h"

pMessage::pMessage(int size, char *buf, char* encBuf) {
	description = "Chat Message (Server)";
	type = ptMessage;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &color);
	getWord(4, &channel);
	getWord(6, &effect);
	getInt(8, &time);
	hour = time / 100;
	min = time % 100;

	getString(12, emotion, 16);

	char* buffer = nullptr;
	int pos = 28;
	int strings = 0;
	int len = 0;
	getByte(pos++, &strings);
	
	getByte(pos++, &len);
	if (len > 0) {
		buffer = new char[len];
		getString(pos, buffer, len);
		pos += len;
		sender = std::string(buffer, len);
		delete[] buffer;
	}

	getByte(pos++, &len);

	if (len > 0) {
		buffer = new char[len];
		getString(pos, buffer, len);
		pos += len;
		target = std::string(buffer, len);
		delete[] buffer;
	}

	getByte(pos++, &len);
	if (len > 0) {
		buffer = new char[len];
		getString(pos, buffer, len);
		pos += len;
		message = std::string(buffer, len);
		delete[] buffer;
	}
}


pMessage::pMessage(int channel, std::string sender, std::string target, std::string message, int color, int effect, std::string emotion) {
	description = "Chat Message (Client)";
	type = ptMessage;

	int size = 36;
	int senderLen = sender.length(); 
	if (senderLen > 16) senderLen = 16;
	int targetLen = target.length();
	if (targetLen > 16) targetLen = 16;
	int messageLen = message.length();
	if (messageLen > 255) messageLen = 255;
	size += senderLen + targetLen + messageLen;

	initBuffer(size);

	this->color = color;
	addInt(0, color);
	this->channel = channel;
	addWord(4, channel);
	this->effect = effect;
	addWord(6, effect);

	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	time = (now->tm_hour * 100) + now->tm_min;
	addInt(8, time);

	char e[16];
	for (int i = 0; i < 16; i++) e[i] = 0;
	memcpy(this->emotion, emotion.c_str(), emotion.length());
	addString(12, this->emotion, 16);

	int nextPos = 28;
	this->sender = sender;
	this->target = target;
	this->message = message;
	packString(sender.c_str(), senderLen);
	packString(target.c_str(), targetLen);
	packString(message.c_str(), messageLen);
	addPack(nextPos);
}


pMessage::~pMessage() {
	//
}

void pMessage::process() {
	//chat->AddMessage(this);
	messageManager.Push(this);
}


void pMessage::debugPrint() {
	Packet::debugPrint();

	//std::cout << "Face: " << npcFace << " Mode: " << mode << std::endl;
	//std::cout << std::endl;
}