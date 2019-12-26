#pragma once

#include "Widget.h"

enum TabStyle
{
	tsSimple,
	tsDetail,
	tsButton
};

class CTabControl : public CWidget {
public:
	CTabControl(CWindow* window, std::string name, int x, int y);
	CTabControl(CWindow* window, rapidjson::Value& vWidget);
	~CTabControl();

	void Render();

private:
	SDL_Texture *tabControlTexture;
	std::vector<Texture*> tabTextures;

	void RenderTabs();
	void RenderButtonTabs();

	void CreateTabControlTexture();
	void DrawSimpleBorder();
	void DrawDetailBorder();
	void DrawButtonBorder();

	void CreateTabTextures();
	void CreateTabTexture(int index);
	void DrawSimpleTab(int index, bool focused);
	void DrawDetailTab(int index, bool focused);
	void DrawButtonTab(int index, bool focused);

	int VisibleTab;
	TabStyle Style;
	bool TabsOnBottom;

	const int TAB_MIN_WIDTH = 44;
	const int TAB_HEADER_HEIGHT = 16;
	const int TAB_BUTTON_HEIGHT = 24;
	std::vector<std::string> tabs;
};
