#include "stdafx.h"
#include "NPC.h"
#include "pNpcInfo.h"
#include "GameMap.h"

NPC::NPC(pNpcInfo* packet) {
	renderer = map->renderer;
	name = packet->name;
	//nameLabel = new Label(name, x, y, true);
	//nameLabel->setFontColor(SDL_Color{ 255,255,255,255 });

	id = packet->id;
	type = packet->npcType;
	look = packet->look;
	x = packet->posX;
	y = packet->posY;
	real_position = map->CenterOfCoord(x, y);
	memcpy(hslSets, packet->hslSets, 15);

	animDir = look % 10;
	direction = animDir;
	//if (direction > 7) direction -= 8;

	loadAnimation(animDir);
}


NPC::~NPC() {
	for (int i = 0; i < 8; i++) {
		if (animations[i]) delete animations[i];
	}

	//delete nameLabel;
}


void NPC::render(int offsetX, int offsetY, bool showName = false) {
	Sprite* renderSprite = getCurrentSprite();
	if (!renderSprite->started) renderSprite->start();
	renderSprite->render(offsetX, offsetY);

	if (showName) {
		int toX = renderSprite->x + offsetX - (nameLabel->width / 2);
		int toY = renderSprite->y + offsetY - 100;
		nameLabel->setPosition(toX, toY);
		nameLabel->render();
	}
}


void NPC::loadAnimation(int dir) {
	if (animations[animDir]) return;
	if (!gClient.npcRle) return;
	
	//int start = SDL_GetTicks();
	int newLook = ((look / 10) * 10) + dir;
	char buf[30];
	sprintf(buf, "npc%04d\0", newLook);

	INI* npcRle = gClient.npcRle;
	npcRle->setSection(std::string(buf));

	//INI roleINI("ANI\\NpcRle.ani", std::string(buf));
	std::vector<Asset> frames;
	if (npcRle->currentSection >= 0) {
		std::string type;
		if (npcRle->getEntry("Type", &type)) {
			if (type == "pet") {
				std::string ani;
				npcRle->getEntry("Ani", &ani);

				std::string aniFile = "ANI\\" + ani;
				std::string aniSection = animationTypeToString(StandBy) + std::to_string(animDir);

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
	//int cur = SDL_GetTicks() - start;
	//std::cout << "NPC Ani ticks: " << cur << std::endl;
	Sprite* temp = new Sprite(renderer, frames, stNpc, hslSets, 3);
	temp->setLocation(real_position.x, real_position.y);
	animations[animDir] = temp;
	//animations[animDir]->setHslShifts(hslSets, 3);
}


Sprite* NPC::getCurrentSprite() {
	return animations[animDir];
}

int NPC::getId() {
	return id;
}

int NPC::getType() {
	return type;
}