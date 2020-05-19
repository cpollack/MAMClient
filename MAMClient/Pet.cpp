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

std::string Pet::getPortraitPath() {
	std::string look = std::to_string(GetLook());
	while (look.size() < 4) look.insert(look.begin(), '0');
	look = "peticon" + look;

	INI ini("INI\\petLook.ini", look);
	return ini.getEntry("Frame0");
}

std::string Pet::getDetails() {
	std::string details;

	details = Name + "\n";
	details += "Level " + formatInt(Level) + "\n";
	details += "Loyalty " + formatInt(loyalty > 100 ? 100 : loyalty) + "\n";
	details += "Life " + formatInt(life_current) + "/" + formatInt(life_max) + "\n";
	details += "Attack " + formatInt(attack) + "\n";
	details += "Defence " + formatInt(defence) + "\n";
	details += "Dexterity " + formatInt(dexterity) + "\n";

	return details;
}

Asset Pet::GetMouseoverTexture(SDL_Renderer *renderer, bool showIcon) {
	int boxWidth = 110;
	Asset detailsTexture(stringToTexture(renderer, getDetails(), gui->font, 0, { 255,255,255,255 }, boxWidth));

	int offsetText = 0;
	if (showIcon) offsetText = 80;
	int height = detailsTexture->height + offsetText;
	Asset finalTexture(new Texture(renderer, nullptr, detailsTexture->width, height));

	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, finalTexture->getTexture());

	SDL_SetTextureBlendMode(finalTexture->getTexture(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
	SDL_RenderClear(renderer);

	if (showIcon) {
		Asset portrait(new Texture(renderer, getPortraitPath(), true));
		if (portrait->loaded) {
			SDL_Rect portRect = { boxWidth / 2 - (portrait->width / 2) , 6, portrait->width, portrait->height };
			SDL_RenderCopy(renderer, portrait->getTexture(), NULL, &portRect);
		}
	}

	SDL_Rect textRect = detailsTexture->rect;
	textRect.y += offsetText;
	SDL_RenderCopy(renderer, detailsTexture->getTexture(), nullptr, &textRect);

	SDL_SetRenderTarget(renderer, priorTarget);
	return finalTexture;
}