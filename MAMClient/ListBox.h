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
	virtual void OnHoverStart(SDL_Event &e);
	virtual void OnHoverEnd(SDL_Event &e);
	void OnListItemHoverStart(SDL_Event& e);
	void OnListItemHoverEnd(SDL_Event& e);
	void OnClick(SDL_Event& e);
	void SelectionChange(SDL_Event& e);
	void SelectionDblClick(SDL_Event &e);
	void OnItemDragStart(SDL_Event &e);
	void OnItemDragEnd(SDL_Event &e);
	void OnFocusLost();

	void ScrollUp();
	void ScrollDown();
	int GetListWidth();
	int GetListHeight();
	int GetItemsHeight();
	SDL_Rect GetScrollBar_BarRect();

	void ClearList() { Items.clear(); }
	void AddItem(std::string text);
	void AddItem(CImageBox* item);

	void SetSelectedIndex(int index);
	int GetSelectedIndex() { return Selected; }
	CImageBox* GetSelectedItem() { return Selected >= 0 ? Items[Selected] : nullptr; }

	int GetMouseOverItem() { return MouseOverItem; }

private:
	SDL_Texture *ListBoxTexture;
	void CreateListBoxTexture();
	int captionOffset;

	std::vector<CImageBox*> Items;
	int Selected = -1;
	SDL_Point ClickPoint;

	int MouseOverItem = -1;
	SDL_Point MouseOverPoint;
	Uint32 MouseOverStart = 0;
	bool HoveringListItem = false;
	int DragIndex = -1;
	bool ItemDrag = false;
	
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
