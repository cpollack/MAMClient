#include "stdafx.h"
#include "BattleArray.h"

#include "Texture.h"
#include "Entity.h"

bool BattleArray::Load(const char *file, int t, bool bAlly) {
	std::ifstream is(file, std::fstream::binary);
	if (!is.is_open()) return false;

	is.read((char*)&header, sizeof(BattleArrayHeader));

	for (int i = 0; i < 10; i++) is.read((char*)&entry[i], sizeof(BattleArrayEntry));

	allyArray = bAlly;
	LoadTexture();

	type = t;
	switch (type) {
	case 0:
		break;
	case 1:
		name = "Phoenix";
		break;
	case 2:
		name = "Tiger";
		break;
	case 3:
		name = "Turtle";
		break;
	case 4:
		name = "Kylin";
		break;
	case 5:
		name = "Dragon";
		break;
	}
	pivot = "Fine";
	condition = "Normal";
	attack = 0;
	defense = 0;
	dex = 0;

	return true;
}

void BattleArray::Render() {
	if (!visible) return;

	//render battle array
	if (leader && leader->IsAlive() && texture) {
		SDL_Rect rect;
		if (allyArray) {
			rect.x = leader->GetBattleBasePos().x - header.imageOffsetX;
			rect.y = leader->GetBattleBasePos().y - header.imageOffsetY;
		}
		else {
			rect.x = leader->GetBattleBasePos().x - (texture->width - header.imageOffsetX);
			rect.y = leader->GetBattleBasePos().y - (texture->height - header.imageOffsetY);
		}
		rect.w = texture->width;
		rect.h = texture->height;
		if (allyArray) SDL_RenderCopy(renderer, texture->texture, NULL, &rect);
		else SDL_RenderCopyEx(renderer, texture->texture, NULL, &rect, 0, NULL, (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));
	}

	//Render battle array text
	//Refactor label handling
	/*std::string topText = "Battle Formation: " + battleArray->name + ", Pivot: " + battleArray->pivot + ", Condition: " + battleArray->condition;
	Label topLabel(topText, 0, 0);
	topLabel.setFontColor(SDL_Color{ 255,255,255,255 });


	std::string bottomText = "Attack: " + std::to_string(battleArray->attack) + "%, Defense: " + std::to_string(battleArray->defense) + "%, Dexterity: " + std::to_string(battleArray->dex) + "%";
	Label bottomLabel(bottomText, 0, 0);
	bottomLabel.setFontColor(SDL_Color{ 255,255,255,255 });

	int toX, toY;
	if (battleArray->top) {
	toX = renderRect.x + renderRect.w - 400;
	toY = renderRect.y + 5;
	}
	else {
	toX = renderRect.x + 5;
	toY = renderRect.y + renderRect.h - topLabel.fontRect.h - bottomLabel.fontRect.h - 5;
	}

	//topLabel.x = toX;
	//topLabel.y = toY;
	topLabel.setPosition(toX, toY);
	topLabel.render();

	//bottomLabel.x = toX;
	//bottomLabel.y = toY + topLabel.fontRect.h + 2;
	bottomLabel.setPosition(toX, toY + topLabel.fontRect.h + 2);
	bottomLabel.render();*/
}

SDL_Point BattleArray::GetPosition(int pos, bool bAlly) {
	if (pos < 0 || pos > 9) return{ -1, -1 };

	if (bAlly) { //Ally array is 'inverted' from enemy array
		if (pos > 4) {
			pos = 14 - pos;
		}
		else {
			pos = 4 - pos;
		}
	}

	SDL_Point p = { entry[pos].x, entry[pos].y };

	if (bAlly) {
		p.x = 800 - ((header.tileOffsetX - p.x) * header.tileWidth);
		p.y = 600 - ((header.tileOffsetY - p.y) * header.tileHeight);
	}
	else {
		p.x = 800 - (p.x * header.tileWidth);
		p.y = 600 - (p.y * header.tileHeight);
	}
	return p;
}

SDL_Point BattleArray::GetTargetPosition(int pos, bool bAlly) {
	if (pos < 0 || pos > 9) return{ -1, -1 };

	SDL_Point p = GetPosition(pos, bAlly);

	if (bAlly) {
		p.x += header.tileWidth;
		p.y -= header.tileHeight;
	}
	else {
		p.x -= header.tileWidth;
		p.y += header.tileHeight;
	}
	return p;
}

void BattleArray::LoadTexture() {
	std::string imagePath = header.imagePath;
	if (imagePath.length() == 0) return;
	imagePath = "ArrayData/" + imagePath;

	texture = new Texture(renderer, imagePath, SDL_Color{ 0,0,0,255 });
	SDL_SetTextureAlphaMod(texture->texture, 192);
	SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_ADD);
	visible = true;
}
