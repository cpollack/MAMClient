#pragma once

#include "Widget.h"

class CCheckBox : public CWidget {
public:
	CCheckBox(CWindow* window, std::string name, int x, int y);
	CCheckBox(CWindow* window, rapidjson::Value& vWidget);
	~CCheckBox();

	void Render();
	void HandleEvent(SDL_Event& e);

	bool isChecked();

private:
	void CreateCheckBoxTexture();
	SDL_Texture *cbTexture;
	SDL_Texture *unpressedTexture, *pressedTexture;
	SDL_Rect cbRect;
	bool checked = false;
	bool held = false;

	void OnClick(SDL_Event& e);
};
