#include "stdafx.h"
#include "Client.h"
#include "Packet.h"


Packet::Packet() {
	//
}


Packet::Packet(int type, int size, char* buf, char* encBuf) {
	this->type = type;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	description = "Unknown Packet Type";
	unknownType = true;
}


Packet::~Packet() {
	if (buffer) delete[] buffer;
	if (encryptedBuffer) delete[] encryptedBuffer;
	if (strPack) delete[] strPack;
}

void Packet::process() {
	//
}


void Packet::initBuffer(int len) {
	size = len;
	buffer = new BYTE[len];
	memset(buffer, 0, len);

	encryptedBuffer = new BYTE[size];
	memset(encryptedBuffer, 0, len);
}


BYTE* Packet::getBuffer() {
	if (encryptedBuffer) return encryptedBuffer;
	return buffer;
}

BYTE* Packet::getDecryptedBuffer() {
	return buffer;
}


int Packet::getLength() {
	return size;
}


int Packet::getType() {
	return type;
}


std::string Packet::getDescription() {
	return description;
}

void Packet::addByte(int pos, BYTE val) {
	buffer[pos + 4] = (char)(val);
}


void Packet::addWord(int pos, WORD val) {
	buffer[pos + 4] = (char)(val & 0xFF);
	buffer[pos + 5] = (char)(val >> 8);
}


void Packet::addDWord(int pos, DWORD val) {
	for (int i = 0; i < 4; ++i) {
		buffer[pos + 4 + i] = (char)(val & 0xFF);
		val >>= 8;
	}
}


void Packet::addInt(int pos, int val) {
	addDWord(pos, val);
}


void Packet::addString(int pos, char *str, int len) {
	memcpy(buffer + 4 + pos, str, len);
}


void Packet::getByte(int pos, BYTE *val) {
	*val = buffer[pos + 4];
}


void Packet::getByte(int pos, int *val) {
	getByte(pos, (BYTE*)val);
}


void Packet::getWord(int pos, WORD *val) {
	int v1 = (buffer[pos + 4] & 0xFF);
	int v2 = (buffer[pos + 5] << 8);

	*val = (v1 | v2);
}


void Packet::getWord(int pos, short *val) {
	getWord(pos, (WORD*)val);
}

void Packet::getWord(int pos, int *val) {
	getWord(pos, (WORD*)val);
}


void Packet::getDWord(int pos, DWORD *val) {
	int v1 = (buffer[pos + 4] & 0xFF);
	int v2 = (buffer[pos + 5] << 8);
	int v3 = (buffer[pos + 6] << 16);
	int v4 = (buffer[pos + 7] << 24);

	*val = v1 | v2 | v3 | v4;
}


void Packet::getInt(int pos, int* val) {
	getDWord(pos, (DWORD*)val);
}


void Packet::getString(int pos, char *str, int len) {
	memcpy(str, buffer + 4 + pos, len);
}


void Packet::packString(const char* str, int len) {
	if (!len) return;

	int newLen = packLen + len + 1;
	char *newPack = new char[newLen];
	if (packLen) {
		memcpy(newPack, strPack, packLen);
	}
	memcpy(newPack + packLen, (BYTE*)&len, 1);
	memcpy(newPack + packLen + 1, str, len);
	packLen = newLen;
	delete[] strPack;
	strPack = newPack;
	packCount++;
}


void Packet::addPack(int pos) {
	memcpy(buffer + 4 + pos, (BYTE*)&packCount, 1);
	memcpy(buffer + 4 + pos + 1, strPack, packLen);
}


std::vector<std::string> Packet::getStringPack(int pos) {
	std::vector<std::string> strPack;
	getByte(pos++, &packLen);
	
	char *packBuf = nullptr;
	for (int i = 0; i < packLen; i++) {
		int nextLen = 0;
		getByte(pos++, &nextLen);
		if (nextLen) {
			packBuf = new char[nextLen + 1];
			for (int j = 0; j <= nextLen; j++) packBuf[j] = 0;
			getString(pos, packBuf, nextLen);
			pos += nextLen;
			strPack.push_back(std::string(packBuf));
			delete[] packBuf; packBuf = nullptr;
		}
		else strPack.push_back("");
	}
	return strPack;
}


/*void Packet::send() {
	buffer[0] = (char)(size >> 8);
	buffer[1] = (char)(size & 0xFF);
	buffer[2] = (char)(type >> 8);
	buffer[3] = (char)(type & 0xFF);

	//encrypt packet

	//add to send queue?
}*/

void Packet::encrypt() {
	buffer[0] = (char)(size & 0xFF);
	buffer[1] = (char)(size >> 8);
	buffer[2] = (char)(type & 0xFF);
	buffer[3] = (char)(type >> 8);

	gClient.encryptPacket(encryptedBuffer, buffer, size);
}


void Packet::debugPrint() {
	std::cout << description << " - Type: " << type << " Size: " << size << std::endl;
	#ifdef DEBUG_PACKET
	gClient.writePacketToLog(this);
	#endif // DEBUG_PACKET
	

	if (unknownType) {
		for (int i = 0; i < size; i++) {
			std::cout << buffer[i];
		}
		std::cout << std::endl;

		for (int i = 0; i < size; i++) {
			std::cout << encryptedBuffer[i];
		}
		std::cout << std::endl;

		if (type == 2037) {
			int d1, d2, d3;
			unsigned int w1=0, w2=0, w3=0, w4=0, w5=0;
			getInt(0, &d1);
			getInt(4, &d2);
			getInt(8, &d3);

			getWord(12, (WORD*)&w1);
			getWord(14, (WORD*)&w2);
			getWord(16, (WORD*)&w3);
			getWord(18, (WORD*)&w4);
			getWord(20, (WORD*)&w5);

			std::cout << d1 << " " << d2 << " " << d3 << std::endl;
			std::cout << w1 << " " << w2 << " " << w3 << " " << w4 << " " << w5 << std::endl;
		}

		std::cout << std::endl;
	}
}