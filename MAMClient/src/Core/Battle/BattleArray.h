#pragma once

class Texture;
class Entity;

struct BattleArrayHeader {
	char imagePath[256];
	int imageOffsetX;
	int imageOffsetY;
	int tileWidth;
	int tileHeight;
	int tileOffsetX;
	int tileOffsetY;
};

struct BattleArrayEntry {
	int x;
	int y;
};

class BattleArray {
public:
	BattleArray(SDL_Renderer *r) {
		renderer = r;
		for (int i = 0; i < 256; i++) header.imagePath[i] = 0;
	}
	~BattleArray() {
		if (texture) delete texture;
		if (details) delete details;
	}

	bool Load(const char* file, int type, bool bAlly);
	void Render();

	void SetLeader(Entity *entity) { leader = entity; }

	SDL_Point GetPosition(int pos, bool bAlly);
	SDL_Point GetTargetPosition(int pos, bool bAlly);

	bool GetType() { return type; }
	bool GetVisible() { return visible; }

	BattleArrayHeader header;
	BattleArrayEntry entry[10];

private:
	void LoadTexture();

	SDL_Renderer *renderer;
	bool visible = false;
	bool allyArray;
	Texture *texture = nullptr;
	Texture *details = nullptr;
	int type;
	std::string name;
	std::string pivot;
	std::string condition;
	int attack;
	int defense;
	int dex;
	bool top;

	Entity *leader = nullptr;
	SDL_Point leaderPoint;
};
