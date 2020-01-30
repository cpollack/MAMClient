#include "stdafx.h"
#include "Pet.h"
#include "Define.h"
#include "Client.h"
#include "MainWindow.h"

#include "Player.h"
#include "Inventory.h"

Pet::Pet(SDL_Renderer *renderer, int id, std::string name, int look) : Monster(renderer, id, name, look) {
	ID = BattleID | _IDMSK_PET;
	Type = OBJ_PET;
}

Pet::Pet(pPetInfo* packet) : Monster(NULL, packet->petId, packet->name, packet->look) {
	updateInfo(packet);
}


Pet::~Pet() {

}

void Pet::updateInfo(pPetInfo* packet) {
	ID = packet->petId;
	BattleID = ID | _IDMSK_PET;
	owner = packet->ownerId;

	Name = packet->name;
	Class = packet->monsterClass;
	Look = packet->look;
	setElement();

	Level = packet->level;
	experience = packet->experience;
	nextLevelExp = getLevelUpExperience();
	generation = packet->generation;
	loyalty = packet->loyalty;

	life_current = packet->life_current;
	life_max = packet->life_max;
	attack = packet->attack;
	defence = packet->defence;
	dexterity = packet->dexterity;

	medal_attack = packet->medal_attack;
	medal_defence = packet->medal_defence;
	medal_dexterity = packet->medal_dexterity;

	skillCount = packet->skillCount;
	for (int i = 0; i < MAX_PET_SKILLS; i++) {
		if (i < skillCount) skills[i] = packet->skills[i];
		else skills[i] = 0;
	}
}


void Pet::setElement() {
	Element = Class / 10000;
	if (Element == 1 || Element == 7) {
		Element = (Class % 10000) / 1000;
	}
}


int Pet::GetElement() {
	return Element;
}

std::string Pet::GetElementText() {
	std::string strElement;
	switch (Element) {
	case 2:
		strElement = "Water";
		break;
	case 3:
		strElement = "Fire";
		break;
	case 4:
		strElement = "Metal";
		break;
	case 5:
		strElement = "Wood";
		break;
	case 6:
		strElement = "Earth";
		break;
	}
	return strElement;
}

int Pet::getExperience() {
	return experience;
}

int Pet::getLevelUpExperience() {
	int nextLevel = Level * (Level + 1) * 0.75;
	return nextLevel;
}

int Pet::getLoyalty() {
	return loyalty;
}

int Pet::getAttack() {
	return attack;
}

int Pet::getDefence() {
	return defence;
}

int Pet::getDexterity() {
	return dexterity;
}

int Pet::getMedalAttack() {
	return medal_attack;
}

int Pet::getMedalDefence() {
	return medal_defence;
}

int Pet::getMedalDexterity() {
	return medal_dexterity;
}

Item* Pet::getItem() {
	return item;
}


void Pet::useItem(int itemId) {
	Item* usedItem = player->inventory->getItem(itemId);
	if (!usedItem) {
		std::cout << "Error: Use Item - Could not find item id " << itemId << std::endl;
		return;
	}

	switch (usedItem->getType()) {
	/*case itWeapon:
	case itArmor:
	case itShoes:
	case itAccessory:
	case itHeadwear:
		equipItem(usedItem);
		break;*/

	case itMedicine:
		useMedicine(usedItem);
		break;
	}
}


void Pet::useMedicine(Item* item) {
	int life = item->getLife();
	if (life != 0) {
		life_current += life;
		mainForm->shiftPetHealthGauge(life);
	}

	player->inventory->removeItem(item->GetID());
}