#include "stdafx.h"
#include "Client.h"
#include "Item.h"

#include "ItemDataFile.h"

Item::Item(pItem* packet) {
	id = packet->itemId;
	ownerId = packet->ownerId;

	name = packet->name;
	creator = packet->creator;

	look = packet->look;
	sort = packet->itemSort;
	setType();

	cost = packet->cost;
	level = packet->levelRequired;

	life = packet->life;
	mana = packet->power;
	attack = packet->attack;
	defence = packet->defence;
	dexterity = packet->dexterity;
	
	INI* itemLookINI;
	std::string bigSection = "Big_item" + std::to_string(look);
	itemLookINI = new INI("INI/itemlook.ini", bigSection);
	bigPath = itemLookINI->getEntry("Frame0");
	delete itemLookINI;

	std::string smallSection = "Small_item" + std::to_string(look);
	itemLookINI = new INI("INI/itemlook.ini", smallSection);
	smallPath = itemLookINI->getEntry("Frame0");
	delete itemLookINI;
}

Item::Item(ItemData itemData) {
	//item loads from INI
	id = itemData.itemID;

	ownerId = -1;

	name = itemData.name;
	creator = itemData.inventor;

	look = itemData.look;
	sort = itemData.itemSort;
	setType();

	cost = itemData.cost;
	level = itemData.levelReq;

	life = itemData.life;
	mana = itemData.power;
	attack = itemData.attack;
	defence = itemData.defence;
	dexterity = itemData.dexterity;

	INI* itemLookINI;
	std::string bigSection = "Big_item" + std::to_string(look);
	itemLookINI = new INI("INI/itemlook.ini", bigSection);
	bigPath = itemLookINI->getEntry("Frame0");
	delete itemLookINI;

	std::string smallSection = "Small_item" + std::to_string(look);
	itemLookINI = new INI("INI/itemlook.ini", smallSection);
	smallPath = itemLookINI->getEntry("Frame0");
	delete itemLookINI;
}

Item::Item(int itemId) {
	//item loads from INI
	id = itemId;

	INI itemIni = *gClient.itemIni;
	std::string itemSection = "id" + std::to_string(itemId);
	itemIni.setSection(itemSection);

	ownerId = -1;

	name = itemIni.getEntry("name");
	creator = itemIni.getEntry("inventor");

	look = stoi(itemIni.getEntry("look"));
	sort = stoi(itemIni.getEntry("itemsort"));
	setType();

	cost = stoi(itemIni.getEntry("cost"));
	level = stoi(itemIni.getEntry("levelrequired"));

	life = stoi(itemIni.getEntry("life"));
	mana = stoi(itemIni.getEntry("power"));
	attack = stoi(itemIni.getEntry("attack"));
	defence = stoi(itemIni.getEntry("defence"));
	dexterity = stoi(itemIni.getEntry("dexterity"));

	INI* itemLookINI;
	std::string bigSection = "Big_item" + std::to_string(look);
	itemLookINI = new INI("INI/itemlook.ini", bigSection);
	bigPath = itemLookINI->getEntry("Frame0");
	delete itemLookINI;

	std::string smallSection = "Small_item" + std::to_string(look);
	itemLookINI = new INI("INI/itemlook.ini", smallSection);
	smallPath = itemLookINI->getEntry("Frame0");
	delete itemLookINI;
}


Item::~Item() {

}


void Item::setType() {
	type = sort / 100;

	switch (type) {
	case itWeapon:
		typeText = "Weapon";
		break;
	case itArmor:
		typeText = "Armor";
		break;
	case itShoes:
		typeText = "Shoes";
		break;
	case itAccessory:
		typeText = "Accessory";
		break;
	case itHeadwear:
		typeText = "Headware";
		break;
	case itThrown:
		typeText = "Thrown";
		break;
	case itPoison:
		typeText = "Poison";
		break;
	case itMedicine:
		typeText = "Medicine";
		break;
	}
}

int Item::getType() {
	return type;
}


int Item::getSort() {
	return sort;
}


int Item::getCost() {
	return cost;
}


std::string Item::getTexturePath(int size) {
	if (size == 40) return smallPath;
	if (size == 85) return bigPath;

	return nullptr;
}


std::string Item::getName() {
	return name;
}


std::wstring Item::getDetails() {
	std::string details;

	details = name + "\n";
	details += "Type " + typeText + "\n\n";

	if (life > 0) {
		if (type == itPoison) details += "Life -" + std::to_string(life) + "\n";
		else details += "Life +" + std::to_string(life) + "\n";
	}
	if (mana > 0) {
		details += "Mana +" + std::to_string(mana) + "\n";
	}
	if (attack > 0) {
		details += "Attack +" + std::to_string(attack) + "\n";
	}
	if (defence > 0) {
		details += "Defence +" + std::to_string(defence) + "\n";
	}
	if (dexterity > 0) {
		details += "Dexterity +" + std::to_string(dexterity) + "\n";
	}

	details += "Price $ " + std::to_string(cost) + "\n";
	details += "Maker " + creator + "\n";
	details += "Lvl Required " + std::to_string(level);
	
	return StringToWString(details);
}


std::wstring Item::getShopDetails() {
	std::string details;
	
	details = name + "\n";
	details += "Type " + typeText + "\n";

	if (type == itPoison) details += "Life -" + std::to_string(life) + "\n";
	else details += "Life +" + std::to_string(life) + "\n";
	details += "Mana +" + std::to_string(mana) + "\n";
	details += "Attack +" + std::to_string(attack) + "\n";
	details += "Defence +" + std::to_string(defence) + "\n";
	details += "Dexterity +" + std::to_string(dexterity) + "\n";

	details += "Lvl Required " + std::to_string(level) + "\n";
	details += "Price $ " + std::to_string(cost) + "\n";
	details += "Maker " + creator + "\n";

	return StringToWString(details);
}


int Item::getLife() {
	return life;
}

int Item::getMana() {
	return mana;
}

int Item::getAttack() {
	return attack;
}

int Item::getDefence() {
	return defence;
}

int Item::getDexterity() {
	return dexterity;
}