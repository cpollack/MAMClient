#pragma once

#include "Widget.h"
#include "Label.h"
#include "Texture.h"

enum ButtonType {
	btPress,
	btToggle
};

/*enum BtnTextureType {
	bttDRAWN,
	bttONOFF,
	bttICON
};*/

class CButton : public CWidget {
public:
	CButton(CWindow* window, std::string name, int x, int y);
	CButton(CWindow* window, rapidjson::Value& vWidget);
	~CButton();

	void Load();
	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);

	void SetType(ButtonType type);
	void SetText(std::string value);
	void SetPressedImage(std::string imagePath);
	void SetUnPressedImage(std::string imagePath);
	void SetUseGUI(bool use);

	void Toggle(bool bToggle);
	bool GetToggled() { return toggled; }

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
	bool usingImages = false;

	void OnClick(SDL_Event& e);
	void OnToggle(SDL_Event& e);
	void OnToggleOn(SDL_Event& e);
	void OnToggleOff(SDL_Event& e);
};
