#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "Texture.h"

class GUI {
public:
	//SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
	SDL_Texture* gTexture;

	bool initSuccess = true;
	SDL_Surface* uiSurface = NULL;

	Texture* mainWindow;
	Texture* surface;
	Texture* minBack;
	Texture* topCenter;
	Texture* topCenter_s;
	Texture* left;
	Texture* right;
	Texture* topLeft;
	Texture* topLeft_s;
	Texture* topRight;
	Texture* topRight_s;
	Texture* bottomCenter;
	Texture* bottomLeft;
	Texture* bottomRight;

	Texture* min;
	Texture* close;

	Texture* login_minpic;
	Texture* minlogo;

	Texture* button_basicMsg;
	Texture* button_basicMsgDown;
	Texture* button_pet;
	Texture* button_petDown;

	Texture* fight1, *fight2;
	Texture* chat1, *chat2;

	Texture* heroStatus;
	Texture* chatPanel;
	Texture* selectMag;
	Texture* button_fly_off, *button_fly_on;

	Texture* minMap1, *minMap2;
	Texture* team1, *team2;
	Texture* items1, *items2;
	Texture* pk1, *pk2;
	Texture* record1, *record2;
	Texture* email1, *email2;

	Texture* bg_slot;
	Texture* green_slot;
	Texture* orange_slot;
	Texture* red_slot;
	Texture* yellow_slot;

	Texture* invetoryBg;
	Texture* star_button;
	Texture* line1, *line2;

	SDL_Color fontColor;
	SDL_Color borderColor;
	SDL_Color backColor;
	SDL_Color buttonColor;
	SDL_Color buttonBorderColor;
	SDL_Color buttonFontColor;

	TTF_Font *font = nullptr;
	TTF_Font *fontUni = nullptr;
	TTF_Font *chatFont = nullptr;
	TTF_Font *chatShadowFont = nullptr;
	TTF_Font *font_bold = nullptr;
	TTF_Font *font_battleResult = nullptr;
	TTF_Font *font_npcDialogue = nullptr;
	//GUI_Texture title;
	//std::string titleText;

	//int window_width;
	//int window_height;
	
	GUI(SDL_Renderer* aRenderer, std::string sName);
	GUI::~GUI();
	void GUI::changeSkin(std::string sName);
	void GUI::loadResource(Texture*& aTexture, std::string file, int anchor);
	void GUI::loadFromRenderedText(Texture* aTexture, std::string text, SDL_Color textColor);
	Texture* getSkinTexture(SDL_Renderer* renderer, std::string file, int anchor);

private:
	std::string guiDirectory;
	std::string skin;
	std::vector<Texture*> textures;

	bool closing = false, minning = false;
	bool dragging = false;
	int drag_start_x, drag_start_y;

	void GUI::loadGUI();
};

extern GUI* gui;