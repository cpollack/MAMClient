#include "stdafx.h"
#include "ShopDataFile.h"


CShopDataFile::CShopDataFile() {
	std::ifstream ifs("ini/Shop.Data", std::ios::binary);
	assert(ifs.is_open());

	int shopCount;
	ifs.read((char*)&shopCount, 4);

	ShopData sd;
	for (int i = 0; i < shopCount; i++) {
		ifs.read((char*)&sd, sizeof(ShopData));
		CShop shop;
		shop.data = sd;
		shops.push_back(shop);
	}

	int itemId;
	for (int i = 0; i < shops.size(); i++) {
		for (int j = 0; j < shops[i].data.itemCount; j++) {
			ifs.read((char*)&itemId, 4);
			shops[i].inventory.push_back(itemId);
		}
	}

	ifs.close();
}

CShopDataFile::~CShopDataFile() {
	//
}

void CShopDataFile::Save() {
	std::ofstream ofs("Shop.Data", std::ios::binary);

	int shopCount = shops.size();
	ofs.write((char*)&shopCount, 4);
	for (int i = 0; i < shopCount; i++) {
		ShopData nextShop = shops[i].data;
		ofs.write((char*)&nextShop, sizeof(ShopData));
	}

	for (auto shop : shops) {
		for (int i = 0; i < shop.data.itemCount; i++) {
			int itemId = shop.inventory[i];
			ofs.write((char*)&itemId, 4);
		}
	}

	ofs.close();
}


void CShopDataFile::AddShop(ShopData newShop, int *itemList, int index) {
	CShop cshop;
	cshop.data = newShop;

	if (itemList) {
		for (int i = 0; i < newShop.itemCount; i++) cshop.inventory.push_back(itemList[i]);
	}

	if (index >= 0 || index < shops.size()) {
		std::vector<CShop>::iterator it = shops.begin();
		shops.insert(it + index, cshop);
	}
	else shops.push_back(cshop);
}


void CShopDataFile::EditShop(int index, std::string name, int mapId, int itemCount, int* items) {
	char z = 0;
	for (int i = 0; i < 16; i++) memcpy(shops[index].data.name + i, &z, 1);
	memcpy(shops[index].data.name, name.c_str(), name.length());
	shops[index].data.mapID = mapId;

	shops[index].data.itemCount = itemCount;
	shops[index].inventory.clear();
	if (items) {
		for (int i = 0; i < itemCount; i++) shops[index].inventory.push_back(items[i]);
	}
}


int CShopDataFile::GetInventorySize(int shopIndex) {
	if (shopIndex < 0 || shopIndex > shops.size()) return 0;
	return shops[shopIndex].inventory.size();
}


int CShopDataFile::GetInventoryItem(int shopIndex, int itemIndex) {
	if (shopIndex < 0 || shopIndex > shops.size()) return 0;

	CShop shop = shops[shopIndex];
	if (itemIndex < 0 || itemIndex > shop.inventory.size()) return 0;
	return shop.inventory[itemIndex];
}

bool CShopDataFile::IsShop(std::string name, int mapId) {
	for (auto nextShop : shops) {
		if (strcmp(nextShop.data.name, name.c_str()) == 0 && nextShop.data.mapID == mapId) {
			return true;
		}
	}
	return false;
}

bool CShopDataFile::GetShop(std::string name, int mapId, CShop &shop) {
	for (auto nextShop : shops) {
		if (strcmp(nextShop.data.name, name.c_str()) == 0 && nextShop.data.mapID == mapId) {
			shop = nextShop;
			return true;
		}
	}
	return false;
}