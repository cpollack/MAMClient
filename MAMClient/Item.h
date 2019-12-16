#ifndef __ITEM_H
#define __ITEM_H

#include "Texture.h"
#include "pItem.h"

enum ItemType {
	itWeapon = 0,
	itArmor = 1,
	itShoes = 2,
	itAccessory = 3,
	itHeadwear = 4,
	itThrown = 5,
	itPoison = 6,
	itMedicine = 7
};

class Item {
private:
	int id, ownerId;
	std::string name, creator, typeText;

	int look, sort, type, cost, level;

	int life, mana, attack, defence, dexterity;

	std::string bigPath, smallPath;
	Texture* smallTexture = nullptr, *bigTexture = nullptr;

	void Item::setType();

public:
	Item(pItem* packet);
	Item(int itemId);
	~Item();

	int Item::getId();
	int Item::getType();
	int Item::getSort();
	int Item::getCost();
	Texture* Item::getTexture(int size);
	std::string Item::getName();
	std::string Item::getDetails();
	std::string Item::getShopDetails();

	int Item::getLife();
	int Item::getMana();
	int Item::getAttack();
	int Item::getDefence();
	int Item::getDexterity();
};

#endif
