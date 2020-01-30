#pragma once

struct ShopData {
	char name[16];
	int mapID;
	int itemCount;
};

class CShop {
public:
	ShopData data;
	std::vector<int> inventory;
};

class CShopDataFile {
public:
	CShopDataFile();
	~CShopDataFile();

	void Save();
	void AddShop(ShopData newShop, int *itemList, int index);
	void EditShop(int index, std::string name, int mapId, int itemCount, int* items);

	std::vector<CShop> shops;

	int GetInventorySize(int shopIndex);
	int GetInventoryItem(int shopIndex, int itemIndex);
	bool IsShop(std::string name, int mapId);
	bool GetShop(std::string name, int mapId, CShop &shop);
};