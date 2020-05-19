#pragma once

#include "Widget.h"

class Sprite;

class CImageBox : public CWidget {
public:
	CImageBox(CWindow* window, std::string name, int x, int y);
	CImageBox(CWindow* window, rapidjson::Value& vWidget);
	~CImageBox();

	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);

	void SetImageFromFile(std::string fileImage);
	void SetImageFromSkin(std::string skinImage);
	void SetImage(Texture* image);
	void BindSprite(Sprite* sprite);

	void UseBlackBackground(bool use);

	SDL_Texture* GetTexture() { return ImageBox; }

	void SetAnchor(int anchor) { Anchor = anchor; }

private:
	SDL_Texture* ImageBox = NULL;
	std::string SkinImage;
	std::string DataImage;
	Sprite* sprite = nullptr;

	bool Bordered;
	bool BlackBackground;
	int Anchor;

	SDL_Point ClickPoint;

	void OnMouseMove(SDL_Event &e);
	void OnClick(SDL_Event& e);
	void OnDragStart(SDL_Event& e);
	void OnDragEnd(SDL_Event& e);

	void OnFocusLost();
};