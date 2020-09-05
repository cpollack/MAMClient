#pragma once

#include "Widget.h"

const int DD_ROWSPACER = 4;

struct ddRow {
	SDL_Renderer *renderer;
	SDL_Texture* texture = NULL;
	std::string text;
	SDL_Rect rect;
	bool focused = false;
	ddRow(SDL_Renderer* renderer, std::string txt);
	~ddRow() {
		SDL_DestroyTexture(texture); texture = NULL;
	}
	void setText(std::string txt);
	void setFocused(bool foc);
	void render(SDL_Rect renderRect);
};

class CDropDown : public CWidget {
public:
	CDropDown(CWindow* window, std::string name, int x, int y);
	CDropDown(CWindow* window, rapidjson::Value& vWidget);
	~CDropDown();

	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);

	//void SetText(std::string value);
	void AddRow(std::string row);
	void SetValue(int row);

	std::string GetSelection();

private: //Properties
	bool Detailed;

private:
	void CreateDropdownTexture();
	SDL_Texture *dropdownTexture;
	SDL_Texture *expandTexture;
	SDL_Texture *arrowTexture, *arrowDownTexture, *arrowUpTexture;
	SDL_Rect expandRect, arrowRect;

	int selectedRow = -1;
	std::vector<ddRow> rows;

	bool pressed = false;
	bool held = false;
	bool toggled = false;
	bool mouseOver = false;
	bool expanded = false;

	void SetFocus(bool focus);
	void OnFocus();
	void OnFocusLost();
	void OnMouseMove(SDL_Event& e);
	void OnClick(SDL_Event& e);
	void OnChange(SDL_Event& e);
};
