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

	int x, y;
	int type;
	int frames, currentFrame, frameCounter;
	int frameInterval = 150; //ms delay between frames
	int speed = 1000; //ms
	int repeatMode, repeatCount;
	int LoopTimer = 0; //ms to wait before restarting the loop
	int LastLoopTime = 0;
	//int direction;
	DWORD startTime, lastFrame;
	bool started = false;
	bool visible = true;
	bool isFinished = false;
	BYTE alpha = 0xFF;
	ColorShifts colorShifts;

	Sprite(SDL_Renderer* aRenderer, int spriteType = stCharacter);
	Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType);
	Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType, ColorShifts shifts);
	Sprite(SDL_Renderer* aRenderer, std::string file, int toX, int toY);
	Sprite(SDL_Renderer* aRenderer, std::string* file, int frameCount, int toX, int toY);
	Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType);
	Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType, ColorShifts shifts);
	Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int toX, int toY);
	Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType);
	Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType, ColorShifts shifts);
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

	void SetLoopTimer(int timeMS) { LoopTimer = timeMS; }
	void RandomizeTimerDelay();

	void setCamera(SDL_Rect c);
	void SetX(int iX) { x = iX; }
	void SetY(int iY) { y = iY; }
	void Sprite::setLocation(SDL_Point p);
	void Sprite::setLocation(int toX, int toY);
	void Sprite::setAlpha(BYTE newAlpha);
	void setFrameInterval(int fiv) { frameInterval = fiv; }
	void Sprite::setHsbShifts(ColorShifts shifts);

	SDL_Rect Sprite::getRenderRect();
	SDL_Rect Sprite::getRenderRect(int frame);
	Asset Sprite::getCurrentTexture();

	Sprite* Sprite::copy();
};
#endif
