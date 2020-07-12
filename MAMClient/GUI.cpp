#include "stdafx.h"
#include "GUI.h"
#include "Define.h"

GUI::GUI(SDL_Renderer* aRenderer, std::string sName) {
	gRenderer = aRenderer;
	skin = sName;
	guiDirectory = "GUI\\Skins";
	loadGUI();
}


GUI::~GUI() {
	TTF_CloseFont(font);
	TTF_CloseFont(fontUni);

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
	loadResource(mainWindow, guiPath + "mainwindow.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(surface, guiPath + "surface.jpg", Anchor::ANCOR_BOTTOMLEFT);
	loadResource(minBack, guiPath + "minback.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(topCenter, guiPath + "TopCenter.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(topCenter_s, guiPath + "TopCenter_s.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(left, guiPath + "Left.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(right, guiPath + "Right.bmp", Anchor::ANCOR_TOPRIGHT);
	loadResource(topLeft, guiPath + "TopLeft.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(topLeft_s, guiPath + "TopLeft_s.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(topRight, guiPath + "TopRight.bmp", Anchor::ANCOR_TOPRIGHT);
	loadResource(topRight_s, guiPath + "TopRight_s.bmp", Anchor::ANCOR_TOPRIGHT);
	loadResource(bottomCenter, guiPath + "BottomCenter.bmp", Anchor::ANCOR_BOTTOMLEFT);
	loadResource(bottomLeft, guiPath + "BottomLeft.bmp", Anchor::ANCOR_BOTTOMLEFT);
	loadResource(bottomRight, guiPath + "BottomRight.bmp", Anchor::ANCOR_BOTTOMLEFT);

	loadResource(min, guiPath + "Min.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(close, guiPath + "Close.bmp", Anchor::ANCOR_TOPLEFT);

	std::string randPic = "login_minpic0" + std::to_string((rand()%4) + 1) + ".jpg";
	loadResource(login_minpic, guiPath + randPic, Anchor::ANCOR_TOPLEFT);
	loadResource(minlogo, guiPath + "HL_minlogo.jpg", Anchor::ANCOR_TOPLEFT);
	
	loadResource(button_basicMsg, guiPath + "button_basicMsg.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(button_basicMsgDown, guiPath + "button_basicMsgDown.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(button_pet, guiPath + "button_pet.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(button_petDown, guiPath + "button_petDown.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(fight2, guiPath + "fight-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(fight1, guiPath + "fight-1.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(chat2, guiPath + "chat-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(chat1, guiPath + "chat-1.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(heroStatus, guiPath + "HeroStatus.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(chatPanel, guiPath + "ChatPanel.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(selectMag, guiPath + "SelectMag.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(button_fly_off, guiPath + "button_fly_off.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(button_fly_on, guiPath + "button_fly_on.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(minMap2, guiPath + "minMap-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(minMap1, guiPath + "minMap-1.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(team2, guiPath + "team-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(team1, guiPath + "team-1.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(items2, guiPath + "items-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(items1, guiPath + "items-1.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(pk2, guiPath + "pk-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(pk1, guiPath + "pk-1.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(record2, guiPath + "record-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(record1, guiPath + "record-1.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(email2, guiPath + "email-2.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(email1, guiPath + "email-1.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(bg_slot, guiPath + "bg_slot.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(green_slot, guiPath + "green_slot.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(orange_slot, guiPath + "orange_slot.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(red_slot, guiPath + "red_slot.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(yellow_slot, guiPath + "yellow_slot.jpg", Anchor::ANCOR_TOPLEFT);

	loadResource(invetoryBg, guiPath + "ItemsPic.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(star_button, guiPath + "star_button.bmp", Anchor::ANCOR_TOPLEFT);
	loadResource(line1, guiPath + "line1.jpg", Anchor::ANCOR_TOPLEFT);
	loadResource(line2, guiPath + "line2.jpg", Anchor::ANCOR_TOPLEFT);

	font = TTF_OpenFont("font\\Arial.ttf", 12);
	fontUni = TTF_OpenFont("font\\ARIALUNI.ttf", 12);
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
		aTexture->anchor = Anchor::ANCOR_TOPLEFT;

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