#pragma once

struct ItemData {
	char name[16];
	char inventor[16];
	int buffer1 = 0;
	int cost;
	short look;
	short itemSort;
	int levelReq;
	short life;
	short power;
	short attack;
	short defence;
	short dexterity; //original 58 bytes ends here
	short antiPoison;
	short antiFreeze;
	short antiSleep;
	short antiChaos;
	short buffer2 = 0;
	//unsigned char phrase[8];
	int phrase1 = 1708440268;
	int phrase2 = 436277248;
	int two = 2;
	int fourSevenTwo = 472;
	int itemID; //88 bytes total
};

class CItemDataFile {
public:
	CItemDataFile();
	~CItemDataFile();

	void Save();
	void Add(ItemData item);
	void Delete(int id);

	int count;
	std::map<int, ItemData> items;

	bool GetItemByID(int id, ItemData &itemData);
};