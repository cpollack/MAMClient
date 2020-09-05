#include "stdafx.h"
#include "Player.h"

#include "Client.h"
#include "MainWindow.h"
#include "GameMap.h"
#include "Pet.h"
#include "Inventory.h"
#include "Team.h"


#include "pPlayerInfo.h"
#include "pAction.h"
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
		if (PlayerRole[0] == 'M') {
			PlayerRole = Role.substr(0, 5);
			Face = stoi(Role.substr(4, 1));
		}
		if (PlayerRole[0] == 'W') {
			PlayerRole = Role.substr(0, 7);
			Face = stoi(Role.substr(6, 1));
		}
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
	BattleID = ID;
	Level = packet->level;
	experience = packet->experience;
	life_current = packet->life_current;
	life_max = packet->life_max;
	mana_current = packet->mana_current;
	mana_max = packet->mana_max;

	Rank = packet->rank;
	Reborns = packet->reborns;
	MasterRank = packet->masterType;
	Alignment = packet->alignment;
	FullRank = (MasterRank * 10000) + (Rank * 1000) + (Reborns * 10) + Alignment;
	//get rankDesc from above
	loadAura();

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
	if (walking && atDestCoord() && path.size() == 0) {
		lastPositionPacket = timeGetTime();
		pAction *packet = new pAction(AccountId, ID, Direction, DestCoord.x, DestCoord.y, amNone);
		gClient.addPacket(packet);
	}

	User::step();

	if (walking) {
		Packet* packet = nullptr;
		DWORD systemTime = timeGetTime();
		if (systemTime - lastPositionPacket >= 2000) {
			lastPositionPacket = systemTime;
			packet = new pAction(AccountId, ID, Direction, DestCoord.x, DestCoord.y, amNone);
		}
		if (packet) gClient.addPacket(packet);
	}
}


void Player::jumpTo(SDL_Point coord) {
	if (!map->isCoordWalkable(coord)) return;

	if (!team) User::jumpTo(coord);

	//Send jump packets
	std::vector<Packet*> jumpPackets;
	pDirection* dirPacket = new pDirection(ID, coord.x, coord.y, Direction);
	jumpPackets.push_back(dirPacket);

	if (!team) {
		lastPositionPacket = timeGetTime();
		pAction* actPacket = new pAction(AccountId, ID, Direction, coord.x, coord.y, amJump);
		jumpPackets.push_back(actPacket);
	}
	else {
		setDirectionToCoord(coord);
		loadSprite();
	}
	gClient.addPacket(jumpPackets);
}


void Player::walkTo(SDL_Point coord) {
	bool canWalk = true;
	if (team && team->GetLeader() != this) canWalk = false;
	if (map->isCoordAPortal(Coord)) canWalk = false;
	if ((walking || Flying) && map->isCoordAPortal(DestCoord)) canWalk = false;

	//Send walking packets
	std::vector<Packet*> walkPackets;
	if (canWalk) {
		User::walkTo(coord);

		pWalk* walkPacket = new pWalk(ID, Coord.x, Coord.y, coord.x, coord.y);
		walkPackets.push_back(walkPacket);
	}
	else {
		setDirectionToCoord(coord);
		loadSprite();
		pDirection *dirPacket = new pDirection(ID, Coord.x, Coord.y, Direction);
		walkPackets.push_back(dirPacket);
	}

	//Move this to step?
	if (walking) {
		DWORD systemTime = timeGetTime();
		if (systemTime - lastPositionPacket >= 2000) {
			lastPositionPacket = systemTime;
			pAction* movePacket = new pAction(AccountId, ID, Direction, DestCoord.x, DestCoord.y, amNone);
			walkPackets.push_back(movePacket);
		}
	}
	gClient.addPacket(walkPackets);
}

void Player::SetCoord(SDL_Point coord) {
	Entity::SetCoord(coord);
	map->setMapPos(Position.x, Position.y);
}

void Player::takeNextStep() {
	User::takeNextStep();
	map->setMapPos(Position.x, Position.y);
}

/*int Player::GetExperience() { 
	return experience; 
}

void Player::SetExperience(int iExp) { 
	experience = iExp; 
}*/


int Player::GetLevelUpExperience() {
	int levelUp;
	levelUp = Level * (Level + 1);
	return levelUp;
}

void Player::SetUnusedPoints(int point) {
	point_unused = point;
}

void Player::SetLifePoint(int point) {
	life = point;

	int max = (3 * life) + 30;
	max += (attack + defence + dexterity) / 4;
	max += (5 * attack) / 100;
	max += (5 * defence) / 100;
	max += (5 * dexterity) / 100;
	life_max = max;
}

void Player::SetManaPoint(int point) {
	mana = point;
	mana_max = (3 * mana) + 20;
}

int Player::GetAttack() {
	int atk = attack;

	for (int i = 0; i < EQUIPMENT_SLOTS; i++) {
		if (equipment[i]) atk += equipment[i]->GetAttack();
	}

	return atk < 0 ? 0 : atk;
}

void Player::SetAttackPoint(int point) {
	attack = point;
}

void Player::SetDefencePoint(int point) {
	defence = point;
}

int Player::GetDefence() {
	int def = defence;

	for (int i = 0; i < EQUIPMENT_SLOTS; i++) {
		if (equipment[i]) def += equipment[i]->GetDefence();
	}

	return def < 0 ? 0 : def;
}

void Player::SetDexterityPoint(int point) {
	dexterity = point;
}

int Player::GetDexterity() {
	int dex = dexterity;

	for (int i = 0; i < EQUIPMENT_SLOTS; i++) {
		if (equipment[i]) dex += equipment[i]->GetDexterity();
	}

	return dex < 0 ? 0 : dex;
}


std::string Player::GetWuxingTitle() {
	std::string title;

	return title;
}

std::string Player::GetKungfuTitle() {
	std::string title;

	return title;
}

std::string Player::GetPetRaisingTitle() {
	std::string title;

	return title;
}

std::string Player::GetThieveryTitle() {
	std::string title;

	return title;
}

void Player::addPet(Pet* newPet) {
	petList[petCount++] = newPet;
}


Pet* Player::setActivePet(int petId) {
	activePet = nullptr;
	for (auto nextPet : petList) {
		if (nextPet && nextPet->GetID() == petId) {
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
		if (nextPet->GetID() == petId) {
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
		if (petList[i] && petList[i]->GetID() == petId) {
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


void Player::addItem(Item* item) {
	inventory->addItem(item);
}

void Player::addItem(pItem* packet) {
	inventory->addItem(packet);
}


void Player::removeItem(Item* item) {
	inventory->removeItem(item->GetID());
}


void Player::useItem(int itemId) {
	Item* usedItem = inventory->getItem(itemId);
	if (!usedItem) {
		std::cout << "Error: Use Item - Could not find item id " << itemId << std::endl;
		return;
	}

	switch (usedItem->GetType()) {
	case itWeapon:
	case itArmor:
	case itShoes:
	case itAccessory:
	case itHeadwear:
		equipItem(usedItem);
		break;

	case itPoison:
	case itMedicine:
		//implement usage of item. medicine heals
		useMedicine(usedItem);
		break;
	}
}


void Player::useMedicine(Item* item) {
	int life = item->GetLife();
	if (life != 0) {
		SetLife(life_current + life);
		mainForm->shiftPlayerHealthGauge(life);
	}

	int mana = item->GetMana();
	if (mana != 0) {
		SetMana(mana_current + mana);
		mainForm->shiftPlayerManaGauge(mana);
	}

	inventory->removeItem(item->GetID());
}


void Player::setEquipment(pItem* packet) {
	Item* newItem = new Item(packet);
	if (newItem->GetType() > 4) {
		std::cout << "Error: Set Equipment - Not a valid item type: " << newItem->GetType() << std::endl;
		return;
	}

	equipItem(newItem);
}


void Player::equipItem(Item* item) {
	int slot = item->GetType();
	Item* itemInSlot = equipment[slot];
	inventory->removeItem(item->GetID(), false);
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
	int slot = item->GetType();
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

	int cost = sItem->GetCost();
	cash += (cost / 2);
	inventory->removeItem(itemId, true);
}


void Player::buyItem(int itemId) {
	Item* bItem = inventory->getItem(itemId);
	if (!bItem) {
		std::cout << "Error: Buy Item - Could not find item id " << itemId << std::endl;
		return;
	}

	int cost = bItem->GetCost();
	cash -= cost;
}

Item* Player::GetWeapon() { 
	return equipment[SLOT_WEAPON]; 
}

Item* Player::GetArmor() { 
	return equipment[SLOT_ARMOR]; 
}

Item* Player::GetShoe() { 
	return equipment[SLOT_SHOE]; 
}

Item* Player::GetBodyAccessory() { 
	return equipment[SLOT_BODY]; 
}

Item* Player::GetHeadAccessory() { 
	return equipment[SLOT_HEAD]; 
}

int Player::GetNextAvailableItemID() {
	bool filled[20];
	for (int i = 0; i < 20; i++) filled[i] = false;

	for (int i = 0; i < inventory->getItemCount(); i++) {
		int slot = inventory->getItemInSlot(i)->GetID() % 100;
		filled[slot] = true;
	}

	for (int i = 0; i < 5; i++) {
		if (equipment[i]) {
			int slot = equipment[i]->GetID() % 100;
			filled[slot] = true;
		}
	}

	int openId = -1;
	for (int i = 0; i < 20; i++) {
		if (!filled[i]) {
			openId = _IDMSK_INVITEM + (ID * 100) + i;
			break;
		}
	}
	return openId;
}