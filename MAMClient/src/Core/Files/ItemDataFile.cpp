#include "stdafx.h"
#include "ItemDataFile.h"

CItemDataFile::CItemDataFile() {
	std::ifstream ifs("ini/Item.Data", std::ios::binary);
	assert(ifs.is_open());

	ifs.read((char*)&count, 4);

	ItemData nextItem;
	for (int i = 0; i < count; i++) {
		ifs.read((char*)&nextItem, sizeof(ItemData));
		items[nextItem.itemID] = nextItem;
	}

	ifs.close();
}

CItemDataFile::~CItemDataFile() {
	//
}

void CItemDataFile::Save() {
	std::ofstream ofs("Item.Data", std::ios::binary);

	int count = items.size();
	ofs.write((char*)&count, 4);

	ItemData nextItem;
	for (auto item : items) {
		nextItem = item.second;
		ofs.write((char*)&nextItem, sizeof(ItemData));
	}

	ofs.close();
}

void CItemDataFile::Add(ItemData item) {
	items[item.itemID] = item;
}

void CItemDataFile::Delete(int id) {
	items.erase(id);
}

bool CItemDataFile::GetItemByID(int id, ItemData &itemData) {
	if (id <= 0) return nullptr;

	std::map<int, ItemData>::iterator it = items.find(id);
	if (it != items.end()) {
		itemData = it->second;
		return true;
	}

	return false;
}
