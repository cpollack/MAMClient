#pragma once

#include "Widget.h"

class CImageBox;

class CListBox : public CWidget {
public:
	CListBox(CWindow* window, std::string name, int x, int y);
	CListBox(CWindow* window, rapidjson::Value& vWidget);
	~CListBox();

	void ReloadAssets();
	void Render();
	void Render_ScrollBar();
	void Step();
	void HandleEvent(SDL_Event& e);

	void OnMouseMove(SDL_Event& e);
	void OnClick(SDL_Event& e);
	void SelectionChange(SDL_Event& e);
	void SelectionDblClick(SDL_Event &e);

	void ScrollUp();
	void ScrollDown();
	int GetListWidth();
	int GetListHeight();
	int GetItemsHeight();
	SDL_Rect GetScrollBar_BarRect();

	void ClearList() { Items.clear(); }
	void AddItem(CImageBox* item);

	void SetSelectedIndex(int index);
	int GetSelectedIndex() { return Selected; }
	CImageBox* GetSelectedItem() { return Selected >= 0 ? Items[Selected] : nullptr; }

private:
	SDL_Texture *ListBoxTexture;
	void CreateListBoxTexture();
	int captionOffset = 7;

	std::vector<CImageBox*> Items;
	int Selected = -1;
	
	bool UseScroll = false;
	const int ScrollWidth = 17, ScrollMidX = 8, ScrollMidY = 8;
	const int ScrollBtnHeight = 17;
	int ScrollPos = 0;
	int ScrollHeight = 0;
	SDL_Rect sbRect;
	bool ScrollBtnTHold = false, ScrollBtnTHover = false;
	bool ScrollBtnBHold = false, ScrollBtnBHover = false;
	bool ScrollUpperHover = false, ScrollUpperHold = false;
	bool ScrollLowerHover = false, ScrollLowerHold = false;
	bool ScrollBarHover = false, ScrollBarDrag = false;
	const int StepMs = 250;
	int LastStepMs = 0;
	int lastDragY;
};
