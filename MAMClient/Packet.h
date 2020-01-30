#pragma once

enum PacketType {
	ptGsConnect = 1002,
	ptMessage = 1004, //change to Talk
	ptWalk = 1005,
	ptEmotion = 1006,
	ptAction = 1007,
	ptPlayerInfo = 1008,
	//ptAttack = 1009,
	//ptSteal = 1010,
	//ptSchool = 1011,
	//ptExchange = 1012,
	//ptDie = 1013,
	//ptReborn = 1013,
	//ptRegister = 115,
	ptItem = 1016, //rename itemInfo
	ptItemAction = 1017, //rename item
	ptDirection = 1018,
	//ptAccident = 1019,
	ptTick = 1020,
	//ptMarriage = 1030
	ptMapInfo = 1031,
	ptUserInfo = 1032,
	ptPetAction = 1033,
	ptPetInfo = 1034,
	//ptSkillInfo = 1035,
	//ptHeal = 1036,
	ptRename = 1037,
	ptWeather = 1038,
	ptUserAttribute = 1040,
	ptUserPoint = 1041,
	ptPetCatalog = 1042,
	ptColor = 1043,
	ptRole = 1050,
	ptLoginRequest = 1051,
	ptLoginResponse = 1052,
	//ptDeleteRole = 1053,
	//ptSkill = 1055,
	//ptTrade = 1056,
	//ptConjure = 1057,
	ptBattleAction = 2000,
	ptNormalAct = 2001, //rename normal act
	//ptSpecialAct = 2002, 
	ptItemAct = 2004,
	ptBattleState = 2005,
	ptEnemyInfo = 2007,
	ptFighterInfo = 2008, //friend?
	ptBattleResult = 2009,
	ptUserLevelUp = 2010,
	ptPetLevelUp = 2011, //rename pet level up
	ptBattleRound = 2012, //end turn
	//ptTeam = 2020,
	//ptTeamInfo = 2021,
	ptFriend = 2022,
	ptNpcInfo = 2030,
	ptNpc = 2031,
	ptNpcAction = 2032, //TaskBar
	ptNpcDialogue = 2033, //TaskBarInfo
	 //ptRemoteAttack = 2034,
	//ptRemoteAttackInfo = 2035,
	//ptPetKeeper = 2036,
	ptColosseum = 2037,
	//ptDuelCommand = 2038,
	//ptWresteler = 2039,
	//ptItemKeeper = 2040,
	//ptDice = 2050,
	//ptBoBet = 2051,
	//ptSyndicateOperation = 2052,
	//ptSyndicateCreate = 2053,
	//ptSndicate = 2054,
	//ptCallPet = 2056, //withdraw?
	ptPetShop = 2057
	//ptCasino = 2058
	//ptCasinoWheel = 2059
	//ptEffect = 2070,
	//ptQuiz = 2071,
	//ptPetMagic = 2072
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
	void Packet::getWord(int pos, short *val);
	void Packet::getWord(int pos, int *val);
	void Packet::getDWord(int pos, DWORD *val);
	void Packet::getInt(int pos, int *val);
	void Packet::getString(int pos, char *str, int len);
	void Packet::getArray(int pos, BYTE *str, int len);

	void packString(const char* str, int len);
	void addPack(int pos);
	char *strPack = nullptr;
	int packLen = 0;
	int packCount = 0;
	std::vector<std::string> getStringPack(int pos);

public:
	Packet();
	Packet(int type, int size, char* buf, char* encBuf);
	virtual ~Packet();

	virtual void process();

	BYTE* Packet::getBuffer();
	BYTE* Packet::getDecryptedBuffer();
	int Packet::getLength();
	int Packet::getType();
	std::string getDescription();
	void Packet::encrypt();

	virtual void Packet::debugPrint();
};
