#pragma once

#include "Widget.h"

class Sprite;

class CImageBox : public CWidget {
public:
	CImageBox(CWindow* window, std::string name, int x, int y);
	CImageBox(CWindow* window, rapidjson::Value& vWidget);
	~CImageBox();

	void Render();
	void HandleEvent(SDL_Event& e);

	void SetImageFromSkin(std::string skinImage);
	void SetImage(Texture* image);
	void BindSprite(Sprite* sprite);

private:
	SDL_Texture* ImageBox = NULL;
	std::string SkinImage;
	Sprite* sprite = nullptr;

	bool Bordered;
	bool BlackBackground;
	
};