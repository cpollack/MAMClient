#include "stdafx.h"
#include "NPC.h"
#include "pNpcInfo.h"
#include "pNpc.h"

#include "CustomEvents.h"
#include "MainWindow.h"
#include "GameMap.h"

NPC::NPC(pNpcInfo* packet):Entity(mainForm->renderer, packet->id, packet->name, packet->look) {
	renderer = map->renderer;
	Name = packet->name;
	nameLabel = stringToTexture(renderer, Name, gui->font, 0, SDL_Color{ 255,255,0,255 }, 0);

	ID = packet->id;
	Type = packet->npcType;
	Look = packet->look;
	setCoord(SDL_Point{ packet->posX, packet->posY });

	memcpy(hslSets, packet->hslSets, 15);

	Direction = Look % 10;

	loadSprite();
}


NPC::~NPC() {
	//
}


void NPC::render() {
	Entity::render();

	if (MouseOver) renderNameplate();
}


void NPC::handleEvent(SDL_Event& e) {
	Entity::handleEvent(e);

	if (!sprite) return;

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_RIGHT && MouseOver) {
			SDL_Event interact;
			SDL_zero(interact);
			interact.type = CUSTOMEVENT_NPC;
			interact.user.data1 = new int(ID);
			interact.user.data2 = nullptr;

			if (Type >= 100 && Type <= 102) {
				int shopId = GetShopID();
				//mainForm->openShop(shopId);
				interact.user.code = NPC_SHOP;
				//interact.user.data2 = new int(shopId);
			}
			else {
				pNpc* npcPack = new pNpc(ID, 0, 0, 0);
				gClient.addPacket(npcPack);		
				interact.user.code = NPC_INTERACT;
			}
			SDL_PushEvent(&interact);
		}
	}
}


void NPC::loadSprite() {
	if (!gClient.npcRle) return;
	
	//int start = SDL_GetTicks();
	int newLook = ((Look / 10) * 10) + Direction;
	char buf[30];
	sprintf(buf, "npc%04d\0", newLook);

	INI* npcRle = gClient.npcRle;
	npcRle->setSection(std::string(buf));

	std::vector<Asset> frames;
	if (npcRle->currentSection >= 0) {
		std::string type;
		if (npcRle->getEntry("Type", &type)) {
			if (type == "pet") {
				std::string ani;
				npcRle->getEntry("Ani", &ani);

				std::string aniFile = "ANI\\" + ani;
				std::string aniSection = animationTypeToString(StandBy) + std::to_string(Direction);

				INI aniIni(aniFile, aniSection);
				for (int i = 0; i < stoi(aniIni.getEntry("FrameAmount")); i++) {
					std::string nextFrame = "Frame" + std::to_string(i);

					std::string framePath = aniIni.getEntry(nextFrame);
					frames.push_back(Asset(new Texture(renderer, framePath, false)));
				}
			}
		}
		else {
			int frameCount = stoi(npcRle->getEntry("FrameAmount"));
			for (int i = 0; i < frameCount; i++) {
				frames.push_back(Asset(new Texture(renderer, npcRle->getEntry("Frame" + std::to_string(i)), false)));
			}
		}
	}
	
	sprite = new Sprite(renderer, frames, stNpc, hslSets, 3);
	sprite->setLocation(Position.x, Position.y);
}

int NPC::GetShopID() {
	std::ifstream ifs("INI\\Shop.Data");

	char* buffer = new char[24];
	ifs.read(buffer, 4);
	int entryCount = *(int*)buffer;

	std::string shopDesc = GetTypeText();
	int shopId = 0;
	for (int i = 0; i < entryCount; i++) {
		ifs.read(buffer, 24);
		char nextShop[16];
		memcpy(nextShop, buffer, 16);
		int nextMap = *(int*)(buffer + 16);
		if (strcmp(nextShop, shopDesc.c_str()) == 0 && map->getMapId() == nextMap) {
			shopId = *(int*)(buffer + 20);
			break;
		}
	}

	delete buffer;
	ifs.close();
	return shopId;
}


std::string NPC::GetTypeText() {
	switch (Type) {
	case 100:
		return "Blacksmith";
	case 101:
		return "Drugstore";
	case 102:
		return "Boutique";
	}

	return "";
}