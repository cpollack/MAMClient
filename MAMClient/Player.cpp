#include "stdafx.h"
#include "Player.h"

#include "Client.h"
#include "MainWindow.h"
#include "GameMap.h"
#include "Pet.h"
#include "Inventory.h"
//#include "GameLibrary.h"


#include "pPlayerInfo.h"
#include "pPosition.h"
#include "pDirection.h"
#include "pWalk.h"

Player::Player(int acctId, int look, int face, char* name):User(mainForm->renderer, acctId, name, look){
	AccountId = acctId;
	Face = face;

	renderer = gClient.renderer;
	inventory = new Inventory();

	/* Role is loaded in Entity 
	   - Set a trimmed Player role for use with Icons and Inventory */
	if (Role.size() > 0) {
		PlayerRole = Role;
		if (PlayerRole[0] == 'M') PlayerRole = Role.substr(0, 5);
		if (PlayerRole[0] == 'W') PlayerRole = Role.substr(0, 7);
	}

	setDirection(0);
	loadSprite();
}


Player::~Player() {
	//
}


void Player::setPlayerInfo(pPlayerInfo* packet) {
	Name = (char*)packet->name;
	NickName = (char*)packet->nickName;
	Spouse = (char*)packet->spouse;

	ID = packet->userId;
	Level = packet->level;
	experience = packet->experience;
	life_current = packet->life_current;
	life_max = packet->life_max;
	mana_current = packet->mana_current;
	mana_max = packet->mana_max;

	Rank = packet->rank;
	Reborns = packet->reborns;
	MasterType = packet->masterType;
	Alignment = packet->alignment;
	FullRank = (MasterType * 10000) + (Rank * 1000) + (Reborns * 10) + Alignment;
	//get rankDesc from above

	pkEnabled = packet->pkEnabled;
	cash = packet->cash;
	reputation = packet->reputation;
	cultivation = packet->cultivation;
	wuxing = packet->wuxing;
	kungfu = packet->kungfu;
	petRaising = packet->petRaising;
	thievery = packet->thievery;

	point_unused = packet->unused_point;
	life = packet->life_point;
	mana = packet->mana_point;
	attack = packet->attack_point;
	defence = packet->defence_point;
	dexterity = packet->dexterity_point;

	if (packet->syndicateId > 0) {
		SyndicateId = packet->syndicateId;
		SubGroudId = packet->syndicateSubId;
		SyndicateRank = packet->syndicateRank;

		Guild = (char*)packet->syndicate;
		SubGroup = (char*)packet->branch;
		GuildTitle = (char*)packet->position;
	}
}


void Player::render() {
	User::render();
	if (!map) return;

	//Player specific rendering
}


void Player::step() {
	User::step();

	if (walking) {
		Packet* packet = nullptr;
		DWORD systemTime = timeGetTime();
		if (systemTime - lastPositionPacket >= 2000) {
			lastPositionPacket = systemTime;
			packet = new pPosition(AccountId, ID, Direction, DestCoord.x, DestCoord.y, 0);
		}
		if (packet) gClient.addPacket(packet);
	}
}


void Player::jumpTo(SDL_Point coord) {
	if (!map->isCoordWalkable(coord)) return;

	User::jumpTo(coord);

	//Send jump packets
	std::vector<Packet*> jumpPackets;
	pDirection* jumpPacket = new pDirection(ID, coord.x, coord.y, Direction);
	jumpPackets.push_back(jumpPacket);

	lastPositionPacket = timeGetTime();
	pPosition* movePacket = new pPosition(AccountId, ID, Direction, coord.x, coord.y, 2);
	jumpPackets.push_back(movePacket);
	gClient.addPacket(jumpPackets);
}


void Player::walkTo(SDL_Point coord) {
	User::walkTo(coord);

	//Send walking packets
	std::vector<Packet*> walkPackets;
	pWalk* walkPacket = new pWalk(ID, Coord.x, Coord.y, coord.x, coord.y);
	walkPackets.push_back(walkPacket);

	//Move this to step?
	if (walking) {
		DWORD systemTime = timeGetTime();
		if (systemTime - lastPositionPacket >= 2000) {
			lastPositionPacket = systemTime;
			pPosition* movePacket = new pPosition(AccountId, ID, Direction, DestCoord.x, DestCoord.y, 0);
			walkPackets.push_back(movePacket);
		}
	}
	gClient.addPacket(walkPackets);
}

void Player::setCoord(SDL_Point coord) {
	Entity::setCoord(coord);
	map->setMapPos(Position.x, Position.y);
}

void Player::takeNextStep() {
	User::takeNextStep();
	map->setMapPos(Position.x, Position.y);
}


int Player::getLevelUpExperience() {
	int levelUp;
	levelUp = Level * (Level + 1);
	return levelUp;
}


/*void Player::loadEffects() {
	INI commonIni("INI\\Common.ini", "FlashDown");
	std::vector<std::string> frames;

	//Flash-down Jump Animation
	for (int i = 0; i < stoi(commonIni.getEntry("FrameAmount")); i++) {
		std::string nextFrame = "Frame" + std::to_string(i);
		std::string path = commonIni.getEntry(nextFrame);
		path = "data\\" + path.substr(2);
		frames.push_back(path);
	}
	Sprite* flashdown = new Sprite(renderer, frames, stEffect);
	flashdown->speed = 2;
	//for (int i = 0; i < stoi(commonIni.getEntry("FrameAmount")); i++) SDL_SetTextureBlendMode(flashdown->subimages.at(i)->texture, SDL_BLENDMODE_ADD);
	for (int i = 0; i < stoi(commonIni.getEntry("FrameAmount")); i++) flashdown->subimages.at(i)->setBlendMode(SDL_BLENDMODE_ADD);
	effects["FlashDown"] = flashdown;
}*/


void Player::addPet(Pet* newPet) {
	petList[petCount++] = newPet;
}


Pet* Player::setActivePet(int petId) {
	for (auto nextPet : petList) {
		if (nextPet && nextPet->getId() == petId) {
			activePet = nextPet;
			break;
		}
	}
	return activePet;
}


Pet* Player::getActivePet() {
	return activePet;
}


Pet* Player::getPet(int petId) {
	for (auto nextPet : petList) {
		if (nextPet->getId() == petId) {
			return nextPet;
		}
	}
	return nullptr;
}


std::vector<Pet*> Player::getPetList() {
	std::vector<Pet*> pets;
	for (auto nextPet : petList) {
		if (nextPet) {
			pets.push_back(nextPet);
		}
	}
	return pets;
}


void Player::removePet(int petId) {
	int petPos = -1;
	for (int i = 0; i < 5; i++) {
		if (petList[i] && petList[i]->getId() == petId) {
			delete petList[i];
			petList[i] = nullptr;
			petPos = i;
			petCount--;
		}

		if (petPos > -1 && i > petPos) {
			petList[i - 1] = petList[i];
			petList[i] = nullptr;
		}
	}
}


void Player::addItem(pItem* packet) {
	inventory->addItem(packet);
}

void Player::useItem(int itemId) {
	Item* usedItem = inventory->getItem(itemId);
	if (!usedItem) {
		std::cout << "Error: Use Item - Could not find item id " << itemId << std::endl;
		return;
	}

	switch (usedItem->getType()) {
	case itWeapon:
	case itArmor:
	case itShoes:
	case itAccessory:
	case itHeadwear:
		equipItem(usedItem);
		break;

	case itMedicine:
		//implement usage of item. medicine heals
		useMedicine(usedItem);
		break;
	}
}


void Player::useMedicine(Item* item) {
	int life = item->getLife();
	if (life != 0) {
		life_current += life;
		mainForm->shiftPlayerHealthGauge(life);
	}

	int mana = item->getMana();
	if (mana != 0) {
		mana_current += mana;
		mainForm->shiftPlayerManaGauge(mana);
	}

	inventory->removeItem(item->getId());
}


void Player::setEquipment(pItem* packet) {
	Item* newItem = new Item(packet);
	if (newItem->getType() > 4) {
		std::cout << "Error: Set Equipment - Not a valid item type: " << newItem->getType() << std::endl;
		return;
	}

	equipItem(newItem);
}


void Player::equipItem(Item* item) {
	int slot = item->getType();
	Item* itemInSlot = equipment[slot];
	inventory->removeItem(item->getId(), false);
	equipment[slot] = item;

	if (itemInSlot) {
		inventory->addItem(itemInSlot);
	}
}


void Player::unequipItem(int slot) {
	Item* curSlot = equipment[slot];
	equipment[slot] = nullptr;
	if (curSlot) {
		inventory->addItem(curSlot);
	}
}


void Player::unequipItem(Item* item) {
	int slot = item->getType();
	Item* itemInSlot = equipment[slot];
	equipment[slot] = nullptr;
	if (itemInSlot) {
		inventory->addItem(itemInSlot);
	}
}


void Player::sellItem(int itemId) {
	Item* sItem = inventory->getItem(itemId);
	if (!sItem) {
		std::cout << "Error: Sell Item - Could not find item id " << itemId << std::endl;
		return;
	}

	int cost = sItem->getCost();
	cash += cost;
	inventory->removeItem(itemId, true);
}


void Player::buyItem(int itemId) {
	Item* bItem = inventory->getItem(itemId);
	if (!bItem) {
		std::cout << "Error: Buy Item - Could not find item id " << itemId << std::endl;
		return;
	}

	int cost = bItem->getCost();
	cash -= cost;
}