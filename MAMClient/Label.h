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

class Label : public Widget {
public:
	SDL_Rect textRect;
	LabelAlignment alignment;

	Label(std::string sLine, int toX, int toY, bool isBold = false);
	~Label();
	void Label::initTextRect();
	void Label::setPosition(int px, int py);
	void Label::setText(std::string sText);
	void Label::renderText(std::string sText);

	void Label::render();
	void Label::offsetPosition(SDL_Point offset);
	void Label::offsetPosition(int offsetX, int offsetY);
	void Label::setAlignment(LabelAlignment algn);
	int Label::getHeight();
};
