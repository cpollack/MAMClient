#ifndef __PANEL_H
#define __PANEL_H

#include "Widget.h"

class CPanel : public CWidget {
public:
	CPanel(CWindow* window, std::string name, int x, int y);
	CPanel(CWindow* window, rapidjson::Value& vWidget);
	~CPanel();

	void ReloadAssets();
	void Render();

private:
	SDL_Texture *panelTexture;
	void CreatePanelTexture();
};

class Label;

class Panel : public Widget {
public:
	Panel(int tox, int toy, int w, int h, std::string header);
	~Panel();

	void Panel::render();

private:
	SDL_Texture* renderTexture;
	SDL_Rect renderRect;
	Label* lblHeader = nullptr;

	std::string panelHeader;
};

#endif
