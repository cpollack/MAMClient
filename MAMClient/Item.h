#ifndef __ITEM_H
#define __ITEM_H

#include "Define.h"
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

class ItemData;

class Item {
private:
	int id, ownerId;
	std::string name, creator, typeText;

	int look, sort, type, cost, level;

	int life, mana, attack, defence, dexterity;

	std::string bigPath, smallPath;
	//Texture* smallTexture = nullptr, *bigTexture = nullptr;

	void Item::setType();

public:
	Item(pItem* packet);
	Item(ItemData itemData);
	Item(int itemId);
	~Item();

	int GetID() { return id; }
	void SetID(DWORD newId) { id = newId; }
	bool IsPetItem() { return id > _IDMSK_INVITEM; }

	int Item::getType();
	int Item::getSort();
	int Item::getCost();
	int getLevel() { return level; }
	std::string getTexturePath(int size);
	std::string Item::GetName();
	std::wstring Item::getDetails();
	std::wstring Item::getShopDetails();

	Asset GetMouseoverTexture(SDL_Renderer *renderer, bool showIcon, int opacityPerc = 50);

	int Item::getLife();
	int Item::getMana();
	int Item::getAttack();
	int Item::getDefence();
	int Item::getDexterity();
};

#endif
