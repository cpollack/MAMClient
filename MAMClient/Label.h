#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "Widget.h"

enum LabelAlignment {
	laLeft,
	laCenter,
	laRight
};

enum LabelVAlignment {
	lvaTop,
	lvaCenter,
	lvaBottom
};

class CLabel : public CWidget {
public:
	CLabel(CWindow* window, std::string name, int x, int y);
	CLabel(CWindow* window, rapidjson::Value& vWidget);
	~CLabel();

	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);

	LabelAlignment alignment;
	LabelVAlignment valignment;

	void SetWrapLength(int len);
	void SetBold(bool bold);
	void SetAlignment(LabelAlignment align);
	void SetVAlignment(LabelVAlignment valign);

	SDL_Rect GetTextRect();

private:
	void OnClick(SDL_Event& e);

	SDL_Rect labelRect;
	bool held;
};
