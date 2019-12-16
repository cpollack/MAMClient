#ifndef __INVENTORY_H
#define __INVENTORY_H

#include "Item.h"

class Inventory {
public:
	std::vector<Item*> inventory;

	Inventory();
	~Inventory();

	void Inventory::addItem(pItem* packet);
	void Inventory::addItem(Item* item);
	void Inventory::removeItem(int itemId, bool destroy = true);

	int Inventory::getItemCount();
	Item* Inventory::getItem(int itemId);
	Item* Inventory::getItemInSlot(int pos);
};

#endif
