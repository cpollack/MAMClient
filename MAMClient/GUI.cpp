#include "stdafx.h"
#include "GUI.h"
#include "Define.h"

GUI::GUI(SDL_Renderer* aRenderer, std::string sName) {
	gRenderer = aRenderer;
	skin = sName;
	guiDirectory = "GUI\\Skins";
	loadGUI();

	medal_attack = new Texture(gRenderer);
	medal_attack->loadTexture("GUI\\main\\medal_attack.bmp");
	medal_defence = new Texture(gRenderer);
	medal_defence->loadTexture("GUI\\main\\medal_defence.bmp");
	medal_dexterity = new Texture(gRenderer);
	medal_dexterity->loadTexture("GUI\\main\\medal_dexterity.bmp");
}


GUI::~GUI() {
	TTF_CloseFont(font);

	//delete textures
	for (int i = textures.size() - 1; i >= 0; i--) {
		delete textures[i];
		textures.pop_back();
	}
}



void GUI::changeSkin(std::string sName) {
	skin = sName;

	//cleanup current skin elements

	loadGUI();
}


void GUI::loadGUI() {
	std::string guiPath = guiDirectory + "\\" + skin + "\\";
	loadResource(mainWindow, guiPath + "mainwindow.jpg", Anchor::aTopLeft);
	loadResource(surface, guiPath + "surface.jpg", Anchor::aBottomLeft);
	loadResource(minBack, guiPath + "minback.bmp", Anchor::aTopLeft);
	loadResource(topCenter, guiPath + "TopCenter.bmp", Anchor::aTopLeft);
	loadResource(topCenter_s, guiPath + "TopCenter_s.bmp", Anchor::aTopLeft);
	loadResource(left, guiPath + "Left.bmp", Anchor::aTopLeft);
	loadResource(right, guiPath + "Right.bmp", Anchor::aTopRight);
	loadResource(topLeft, guiPath + "TopLeft.bmp", Anchor::aTopLeft);
	loadResource(topLeft_s, guiPath + "TopLeft_s.bmp", Anchor::aTopLeft);
	loadResource(topRight, guiPath + "TopRight.bmp", Anchor::aTopRight);
	loadResource(topRight_s, guiPath + "TopRight_s.bmp", Anchor::aTopRight);
	loadResource(bottomCenter, guiPath + "BottomCenter.bmp", Anchor::aBottomLeft);
	loadResource(bottomLeft, guiPath + "BottomLeft.bmp", Anchor::aBottomLeft);
	loadResource(bottomRight, guiPath + "BottomRight.bmp", Anchor::aBottomRight);

	loadResource(min, guiPath + "Min.bmp", Anchor::aTopLeft);
	loadResource(close, guiPath + "Close.bmp", Anchor::aTopLeft);

	std::string randPic = "login_minpic0" + std::to_string((rand()%4) + 1) + ".jpg";
	loadResource(login_minpic, guiPath + randPic, Anchor::aTopLeft);
	loadResource(minlogo, guiPath + "HL_minlogo.jpg", Anchor::aTopLeft);
	
	loadResource(button_basicMsg, guiPath + "button_basicMsg.jpg", Anchor::aTopLeft);
	loadResource(button_basicMsgDown, guiPath + "button_basicMsgDown.jpg", Anchor::aTopLeft);

	loadResource(button_pet, guiPath + "button_pet.jpg", Anchor::aTopLeft);
	loadResource(button_petDown, guiPath + "button_petDown.jpg", Anchor::aTopLeft);

	loadResource(fight2, guiPath + "fight-2.jpg", Anchor::aTopLeft);
	loadResource(fight1, guiPath + "fight-1.jpg", Anchor::aTopLeft);

	loadResource(chat2, guiPath + "chat-2.jpg", Anchor::aTopLeft);
	loadResource(chat1, guiPath + "chat-1.jpg", Anchor::aTopLeft);

	loadResource(heroStatus, guiPath + "HeroStatus.jpg", Anchor::aTopLeft);
	loadResource(chatPanel, guiPath + "ChatPanel.jpg", Anchor::aTopLeft);
	loadResource(selectMag, guiPath + "SelectMag.jpg", Anchor::aTopLeft);

	loadResource(button_fly_off, guiPath + "button_fly_off.jpg", Anchor::aTopLeft);
	loadResource(button_fly_on, guiPath + "button_fly_on.jpg", Anchor::aTopLeft);

	loadResource(minMap2, guiPath + "minMap-2.jpg", Anchor::aTopLeft);
	loadResource(minMap1, guiPath + "minMap-1.jpg", Anchor::aTopLeft);
	loadResource(team2, guiPath + "team-2.jpg", Anchor::aTopLeft);
	loadResource(team1, guiPath + "team-1.jpg", Anchor::aTopLeft);
	loadResource(items2, guiPath + "items-2.jpg", Anchor::aTopLeft);
	loadResource(items1, guiPath + "items-1.jpg", Anchor::aTopLeft);
	loadResource(pk2, guiPath + "pk-2.jpg", Anchor::aTopLeft);
	loadResource(pk1, guiPath + "pk-1.jpg", Anchor::aTopLeft);
	loadResource(record2, guiPath + "record-2.jpg", Anchor::aTopLeft);
	loadResource(record1, guiPath + "record-1.jpg", Anchor::aTopLeft);
	loadResource(email2, guiPath + "email-2.jpg", Anchor::aTopLeft);
	loadResource(email1, guiPath + "email-1.jpg", Anchor::aTopLeft);

	loadResource(bg_slot, guiPath + "bg_slot.jpg", Anchor::aTopLeft);
	loadResource(green_slot, guiPath + "green_slot.jpg", Anchor::aTopLeft);
	loadResource(orange_slot, guiPath + "orange_slot.jpg", Anchor::aTopLeft);
	loadResource(red_slot, guiPath + "red_slot.jpg", Anchor::aTopLeft);
	loadResource(yellow_slot, guiPath + "yellow_slot.jpg", Anchor::aTopLeft);

	loadResource(invetoryBg, guiPath + "ItemsPic.jpg", Anchor::aTopLeft);
	loadResource(star_button, guiPath + "star_button.bmp", Anchor::aTopLeft);
	loadResource(line1, guiPath + "line1.jpg", Anchor::aTopLeft);
	loadResource(line2, guiPath + "line2.jpg", Anchor::aTopLeft);

	font = TTF_OpenFont("font\\Arial.ttf", 12);
	chatFont = TTF_OpenFont("font\\Arial.ttf", 12);
	chatShadowFont = TTF_OpenFont("font\\Arial.ttf", 12);
	//TTF_SetFontOutline(chatShadowFont, 1);
	//TTF_SetFontOutline(chatFont, 1);

	//font = TTF_OpenFont("font\\Verdana.ttf", 12);
	//TTF_SetFontOutline(font, 1);
	font_bold = TTF_OpenFont("font\\Verdana.ttf", 12); //Verdana_Bold.ttf
	//TTF_SetFontStyle(font_bold, TTF_STYLE_BOLD);

	font_battleResult = TTF_OpenFont("font\\Verdana.ttf", 11);
	font_npcDialogue = TTF_OpenFont("font\\Verdana.ttf", 12);

	fontColor = { 173, 233, 205, 255 };
	borderColor = { 0, 0, 0, 255 };
	backColor = { 33, 81, 58, 255 };
	buttonColor = { 00, 56, 55, 255};
	buttonBorderColor = { 0, 0, 0, 255 };
	buttonFontColor = { 170, 213, 255, 255};
}


void GUI::loadResource(Texture*& aTexture, std::string file, int anchor) {
	aTexture = new Texture(gRenderer);
	aTexture->setAnchor(anchor);
	aTexture->loadTexture(file);
	textures.push_back(aTexture);
}


void GUI::loadFromRenderedText(Texture* aTexture, std::string text, SDL_Color textColor) {
	SDL_Color black = { 0,0,0 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	if (textSurface == NULL) {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		initSuccess = false;
	}
	else {
		aTexture->texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		aTexture->width = textSurface->w;
		aTexture->height = textSurface->h;
		aTexture->anchor = Anchor::aTopLeft;

		SDL_FreeSurface(textSurface);
	}
}

Texture* GUI::getSkinTexture(SDL_Renderer* renderer, std::string file, int anchor) {
	std::string guiPath = guiDirectory + "\\" + skin + "\\";
	Texture* texture = new Texture(renderer);
	texture->setAnchor(anchor);
	texture->loadTexture(guiPath + file);
	return texture;
}