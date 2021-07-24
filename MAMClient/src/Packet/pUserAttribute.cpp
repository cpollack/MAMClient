#include "stdafx.h"
#include "Client.h"
#include "pUserAttribute.h"

#include "CustomEvents.h"
#include "Player.h"

pUserAttribute::pUserAttribute(int size, char *buf, char* encBuf) {
	description = "User Attribute Message (Server)";
	type = ptUserAttribute;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getInt(0, &userId);
	getInt(4, &count);

	int nextPos = 8;
	for (int i = 0; i < count; i++) {
		AttributeChange attr;
		getString(nextPos, (char*)&attr, 8);
		changes.push_back(attr);
	}
}


pUserAttribute::~pUserAttribute() {
	//
}

void pUserAttribute::process() {
	if (player->GetID() != userId) return;

	SDL_Event e;
	SDL_zero(e);
	e.type = CUSTOMEVENT_PLAYER;

	for (int i = 0; i < count; i++) {
		AttributeChange nextChange = changes[i];
		switch (nextChange.code) {
			case UPD_LIFECURRENT:
				player->SetLife(nextChange.value);
				e.user.code = PLAYER_LIFE;
				break;
			case UPD_LIFEMAX:
				player->SetMaxLife(nextChange.value);
				e.user.code = PLAYER_LIFEMAX;
				break;
			case UPD_MANACURRENT:
				player->SetMana(nextChange.value);
				e.user.code = PLAYER_MANA;
				break;
			case UPD_MANAMAX:
				player->SetMaxMana(nextChange.value);
				e.user.code = PLAYER_MANAMAX;
				break;
			case UPD_MONEY:
				player->SetCash(nextChange.value);
				e.user.code = PLAYER_MONEY;
				break;
			case UPD_EXP:
				player->SetExperience(nextChange.value);
				e.user.code = PLAYER_EXP;
				break;
			case UPD_REPUTATION:
				player->SetReputation(nextChange.value);
				e.user.code = PLAYER_REPUTATION;
				break;
			case UPD_THIEVERY:
				player->SetThievery(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_KUNGFU:
				player->SetKungfu(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_PETRAISING:
				player->SetPetRaising(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_WUXING:
				player->SetWuxing(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_DEPOSITED:
				//?
				break;
			case UPD_ATTRPOINT:
				player->SetUnusedPoints(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_LOOK:
				player->SetLook(nextChange.value);
				e.user.code = PLAYER_LOOK;
				break;
			case UPD_LEVEL:
				player->SetLevel(nextChange.value);
				e.user.code = PLAYER_LEVEL;
				break;
			case UPD_PHYSIQUE:
				player->SetLifePoint(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_STAMINA:
				player->SetDefencePoint(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_FORCE:
				player->SetAttackPoint(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_SPEED:
				player->SetDexterityPoint(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_DEGREE:
				player->SetManaPoint(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_PK:
				player->SetPK(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_EMOTION:
				//?
				break;
			case UPD_CULTIVATION:
				player->SetCultivation(nextChange.value);
				e.user.code = PLAYER_UPDATE;
				break;
			case UPD_LOCKKEY:
				//?
				break;
			case UPD_ALIGNMENT:
				player->SetAlignment(nextChange.value);
				e.user.code = PLAYER_UPDATE;
				break;
			case UPD_RANK:
				player->SetRank(nextChange.value);
				e.user.code = PLAYER_UPDATE;
				break;
			case UPD_VIRTUE:
				player->SetVirtue(nextChange.value);
				e.user.code = PLAYER_UPDATEDETAILS;
				break;
			case UPD_REBORN:
				player->SetReborns(nextChange.value);
				e.user.code = PLAYER_UPDATE;
				break;
		}
	}

	if (e.user.code != 0) SDL_PushEvent(&e);
}


void pUserAttribute::debugPrint() {
	Packet::debugPrint();

	std::cout << "User:" << userId << " Count:" << count << std::endl;

	for (int i = 0; i < count; i++) {
		std::cout << "[" << (i+1) << "] Type:" << changes[i].code << " Value:" << changes[i].value << std::endl;
	}
	std::cout << std::endl;
}