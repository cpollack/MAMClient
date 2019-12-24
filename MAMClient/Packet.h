#pragma once

enum PacketType {
	ptGsConnect = 1002,
	ptMessage = 1004,
	ptWalk = 1005,
	ptEmotion = 1006,
	ptAction = 1007,
	ptPlayerInfo = 1008,
	ptItem = 1016,
	ptItemAction = 1017,
	ptDirection = 1018,
	ptTick = 1020,
	//pMarriage? = 1030
	ptMapInfo = 1031,
	ptUserInfo = 1032,
	ptPetAction = 1033,
	ptPetInfo = 1034,
	//pt1035 = 1035,
	ptWeather = 1038,
	//pt1040 = 1040, ptUserAttribute?
	ptAttributeUpdate = 1043,
	ptPetCatalog = 1042,
	ptColor = 1043,
	ptRole = 1050,
	ptLoginRequest = 1051,
	ptLoginResponse = 1052,
	ptBattleAction = 2000,
	ptBattleResponse = 2001,
	//ptSpecialAct = 2002, // special action in battle?
	ptBattleState = 2005,
	ptEnemyInfo = 2007,
	ptFighterInfo = 2008,
	ptBattleResult = 2009,
	ptPetUpdate = 2011,
	ptBattleRound = 2012,
	ptFriend = 2022,
	ptNpcInfo = 2030,
	ptNpc = 2031,
	ptNpcAction = 2032,
	ptNpcDialogue = 2033,
	ptColosseum = 2037,
	ptPetShop = 2057
};

class Packet {
protected:
	BYTE* buffer = nullptr;
	BYTE* encryptedBuffer = nullptr;
	WORD type;
	WORD size;
	std::string description = "Unknown";
	bool unknownType = false;
	bool hideEncryptedBuffer = false;

	void Packet::initBuffer(int len);

	void Packet::addByte(int pos, BYTE val);
	void Packet::addWord(int pos, WORD val);
	void Packet::addDWord(int pos, DWORD val);
	void Packet::addInt(int pos, int val);
	void Packet::addString(int pos, char *str, int len);

	void Packet::getByte(int pos, BYTE *val);
	void Packet::getByte(int pos, int *val);
	void Packet::getWord(int pos, WORD *val);
	void Packet::getWord(int pos, int *val);
	void Packet::getDWord(int pos, DWORD *val);
	void Packet::getInt(int pos, int *val);
	void Packet::getString(int pos, char *str, int len);

	void packString(const char* str, int len);
	void addPack(int pos);
	char *strPack = nullptr;
	int packLen = 0;
	int packCount = 0;
	std::vector<std::string> getStringPack(int pos);

public:
	Packet();
	Packet(int type, int size, char* buf, char* encBuf);
	~Packet();

	virtual void process();

	BYTE* Packet::getBuffer();
	BYTE* Packet::getDecryptedBuffer();
	int Packet::getLength();
	int Packet::getType();
	std::string getDescription();
	void Packet::encrypt();

	virtual void Packet::debugPrint();
};
