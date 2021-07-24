#pragma once

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
