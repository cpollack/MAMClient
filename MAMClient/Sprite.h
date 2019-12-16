#ifndef _SPRITE_H
#define _SPRITE_H

#include "Client.h"
#include <Windows.h>

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "AssetManager.h"
#include "Texture.h"

enum SpriteType {
	stCharacter,
	stMonster,
	stNpc,
	stAni,
	stObject,
	stEffect,
	stStatic
};

enum RepeatMode {
	LOOP = -1,
	NONE = 0,
};

enum AnimType {
	Attack01,
	Attack02,
	Attack03,
	Cast,
	Defend,
	Faint,
	Fill,
	Fury,
	Genuflect,
	Laugh,
	Lie,
	Politeness,
	Sad,
	SayHello,
	SitDown,
	Specialties_Attack,
	StandBy,
	Walk,
	Wound
};

class Sprite {
public:
	SDL_Renderer* renderer;
	std::vector<std::string> fileList;
	std::vector<Asset> subimages;
	SDL_Rect* spr_rect = nullptr;
	SDL_Rect* camera = nullptr;

	std::map<int, ColorShift> colorShifts;

	int x, y;
	int type;
	int frames, currentFrame, frameCounter;
	int speed; //ms
	int repeatMode, repeatCount;
	//int direction;
	DWORD startTime;
	bool started = false;
	bool visible = true;
	bool isFinished = false;
	BYTE alpha = 0xFF;
	HSBSet hsbSets[7] = { 0 };
	int hsbSetCount = 0;
	int hsbShiftId = 0;

	Sprite(SDL_Renderer* aRenderer, int spriteType = stCharacter);
	Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType);
	Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType, HSBSet* hsbSets, int hsbCount);
	Sprite(SDL_Renderer* aRenderer, std::string file, int toX, int toY);
	Sprite(SDL_Renderer* aRenderer, std::string* file, int frameCount, int toX, int toY);
	Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType);
	Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType, HSBSet* hsbSets, int hsbCount);
	Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int toX, int toY);
	Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType);
	Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType, HSBSet* hsbSets, int hsbCount);
	~Sprite();

	void LoadFirst();
	void Sprite::deleteTextures();
	void Sprite::init(SDL_Renderer* aRenderer, std::vector<std::string> file, int frameCount, int toX, int toY, int spriteType);
	void Sprite::addFrame(Asset newFrame);
	void Sprite::render();
	void Sprite::render(int offsetX, int offsetY);
	void Sprite::start();
	void Sprite::resume();
	void Sprite::stop();
	bool Sprite::finished();

	void setCamera(SDL_Rect c);
	void Sprite::setLocation(SDL_Point p);
	void Sprite::setLocation(int toX, int toY);
	void Sprite::setAlpha(BYTE newAlpha);
	void Sprite::setHsbShifts(HSBSet* sets, int count);

	SDL_Rect Sprite::getRenderRect();
	SDL_Rect Sprite::getRenderRect(int frame);
	Asset Sprite::getCurrentTexture();

	Sprite* Sprite::copy();
};
#endif
