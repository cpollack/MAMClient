#pragma once

#include "Widget.h"
#include "Label.h"
#include "Texture.h"

enum ButtonType {
	btPress,
	btToggle
};

enum BtnTextureType {
	bttDRAWN,
	bttONOFF,
	bttICON
};

class CButton : public CWidget {
public:
	CButton(CWindow* window, std::string name, int x, int y);
	CButton(CWindow* window, rapidjson::Value& vWidget);
	~CButton();

	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);
	void HandleEvent(SDL_Event& e, int mx, int my);

	void SetType(ButtonType type);
	void SetText(std::string value);
	void SetPressedImage(std::string imagePath);
	void SetUnPressedImage(std::string imagePath);
	void SetUseGUI(bool use);

	bool GetToggled();

private: //Properties
	bool UseGUI = false;
	std::string PressedImagePath;
	std::string UnPressedImagePath;

private:
	void CreateButtonTexture();
	SDL_Texture *buttonTexture;
	SDL_Texture *unpressedTexture, *pressedTexture, *mouseoverTexture;
	Texture* pressedImage = nullptr;
	Texture* unpressedImage = nullptr;
	SDL_Rect buttonRect;

	int Type = btPress;
	bool pressed = false;
	bool held = false;
	bool toggled = false;
	bool mouseOver = false;
	bool usingImages = false;

	void OnClick(SDL_Event& e);
};

class Button : public Widget {
public:
	SDL_Texture *lTexture, *iconTexture;
	SDL_Texture *unpressedTexture = nullptr;
	SDL_Texture *pressedTexture = nullptr;
	SDL_Texture *mouseoverTexture = nullptr;
	TTF_Font* lFont;

	SDL_Color fontColor;

	int size;
	int width, iconWidth;
	int height, iconHeight;
	int buttonType, textureType;
	bool visible = true;
	bool held = false, pressed = false;
	std::string text;

	Button(std::string buttonText, int toX, int toY, int w, int h);
	Button(SDL_Texture *offTexture, SDL_Texture *onTexture, int toX, int toY);
	~Button();
	void Button::createButtonTexture();
	bool Button::handleEvent(SDL_Event* e);
	//void Button::renderText(std::string sText);
	//void Button::setText(std::string sText);
	void Button::render();
	void Button::setVisibility(bool setVisible);
	void Button::setButtonText(std::string txt);
	bool Button::wasPressed();
	bool isToggled();

private:
	bool imageButton = false;
	bool mouseOver = false;
};
