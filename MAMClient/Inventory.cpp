#include "stdafx.h"
#include "Inventory.h"


Inventory::Inventory() {
	//
}


Inventory::~Inventory() {
	for (auto item : inventory) {
		delete item;
	}
}

void Inventory::addItem(pItem* packet) {
	Item* newItem = new Item(packet);
	if (!newItem) return;

	inventory.push_back(newItem);
}


void Inventory::addItem(Item* item) {
	inventory.push_back(item);
}


void Inventory::removeItem(int itemId, bool destroy) {
	std::vector<Item*>::iterator itr;
	for (itr = inventory.begin(); itr != inventory.end(); itr++)
	{
		Item* item = *itr;
		if (item->GetID() == itemId) {
			inventory.erase(itr);
			if (destroy) delete item;
			break;
		}
	}
}


int Inventory::getItemCount() {
	return inventory.size();
}


Item* Inventory::getItem(int itemId) {
	for (auto item : inventory) {
		if (item->GetID() == itemId) return item;
	}
	return nullptr;
}


Item* Inventory::getItemInSlot(int pos) {
	if (inventory.size() < pos + 1) return nullptr;
	if (inventory.size() >= pos && inventory[pos]) return inventory[pos];
	return nullptr;
}