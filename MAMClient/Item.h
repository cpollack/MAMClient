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

	void Item::setType();

public:
	Item(pItem* packet);
	Item(ItemData itemData);
	Item(int itemId);
	~Item();

	int GetID() { return id; }
	void SetID(DWORD newId) { id = newId; }
	bool IsPetItem() { return id > _IDMSK_INVITEM; }

	int GetType();
	int GetSort();
	int GetCost();
	std::string getTexturePath(int size);
	std::string GetName();
	std::string GetCreator() { return creator; }
	std::string GetInventor() { return creator; }
	std::wstring getDetails();
	std::wstring getShopDetails();

	Asset GetMouseoverTexture(SDL_Renderer *renderer, bool showIcon, int opacityPerc = 50);

	int GetLevel();
	int GetRealLevel();
	int GetLife();
	int GetMana();
	int GetAttack();
	int GetDefence();
	int GetDexterity();
};

#endif
