#include "stdafx.h"
#include "Window.h"
#include "ListBox.h"
#include "ImageBox.h"

#include "CustomEvents.h"
#include "TabControl.h"

CListBox::CListBox(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;
	captionOffset = 0;
	WidgetType == wtListBox;
}

CListBox::CListBox(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("Caption")) {
		SetText(vWidget["Caption"].GetString());
		captionOffset = 7;
	}
	else captionOffset = 0;

	if (vWidget.HasMember("Widgets")) {
		rapidjson::Value vChildWidgets = vWidget["Widgets"].GetArray();
		if (vChildWidgets.IsArray()) {
			for (rapidjson::SizeType i = 0; i < vChildWidgets.Size(); i++) {
				rapidjson::Value vChildWidget = vChildWidgets[i].GetObject();
				CWidget* newWidget = Window->LoadWidgetByType(vChildWidget);

				AddChild(newWidget);
				newWidget->SetParent(this);
			}
		}
	}
	WidgetType == wtListBox;
}

CListBox::~CListBox() {
	if (ListBoxTexture) SDL_DestroyTexture(ListBoxTexture);

	for (auto item : Items) delete item;
}

void CListBox::ReloadAssets() {
	if (ListBoxTexture) {
		CreateListBoxTexture();
		for (auto widget : Children) {
			widget->ReloadAssets();
		}
	}
}

void CListBox::Render() {
	if (!Visible) return;
	if (!ListBoxTexture) CreateListBoxTexture();
	SDL_RenderCopy(renderer, ListBoxTexture, NULL, &widgetRect);

	//moved to virtual call from window
	//Step(); //Check for list changes each 'step'

	//Render the list
	SDL_Rect oldVp;
	SDL_RenderGetViewport(renderer, &oldVp);

	SDL_Rect listRect = { X + 3, Y + captionOffset + 3 + 3, GetListWidth(), GetListHeight() };
	SDL_RenderSetViewport(renderer, &listRect);
	listRect.x = 0;
	listRect.y = ScrollPos;

	SDL_Point pos = {0,0};
	for (int i = 0; i < Items.size(); i++) {
		CImageBox *ib = Items[i];
		SDL_Rect itemRect = { pos.x, pos.y, ib->GetWidth(), ib->GetHeight() };
		if (doRectIntersect(listRect, itemRect)) {
			itemRect.y -= ScrollPos;
			SDL_RenderCopy(renderer, ib->GetTexture(), NULL, &itemRect);

			if (i == Selected) {
				if (UseScroll) itemRect.w -= ScrollWidth;
				rectangleRGBA(renderer, itemRect.x, itemRect.y, itemRect.x + itemRect.w - 1, itemRect.y + itemRect.h - 1, 255, 255, 0, 40);
				rectangleRGBA(renderer, itemRect.x+1, itemRect.y+1, itemRect.x + itemRect.w-2, itemRect.y + itemRect.h-2, 255, 255, 0, 60);
				rectangleRGBA(renderer, itemRect.x+2, itemRect.y+2, itemRect.x + itemRect.w-3, itemRect.y + itemRect.h-3, 255, 255, 0, 40);
			}
		}
		pos.y += ib->GetHeight();
	}

	if (UseScroll) Render_ScrollBar();

	SDL_RenderSetViewport(renderer, &oldVp);
}

void CListBox::Render_ScrollBar() {
	int white = 255;
	int black = 0;

	//BG
	int bg = 240;
	vlineRGBA(renderer, sbRect.x, sbRect.y, sbRect.y + sbRect.h - 1, white, white, white, 255);
	boxRGBA(renderer, sbRect.x + 1, sbRect.y, sbRect.x + sbRect.w - 2, sbRect.y + sbRect.h - 1, bg, bg, bg, 255);
	vlineRGBA(renderer, sbRect.x + sbRect.w - 1, sbRect.y, sbRect.y + sbRect.h - 1, white, white, white, 255);

	//Buttons
	int btnNone = 240; int arrNone = 96;
	int btnHover = 218; int arrHover = black;
	int btnHold = 96; int arrHold = white;
	int arrColor, btnColor;

	//Top
	arrColor = arrNone;
	btnColor = btnNone;
	if (ScrollBtnTHold) {
		arrColor = arrHold;
		btnColor = btnHold;
	}
	else if (ScrollBtnTHover) {
		arrColor = arrHover;
		btnColor = btnHover;
	}
	SDL_Point m = { sbRect.x + ScrollMidX, sbRect.y + ScrollMidY };
	boxRGBA(renderer, sbRect.x + 1, sbRect.y, sbRect.x + ScrollWidth - 2, sbRect.y + ScrollBtnHeight - 1, btnColor, btnColor, btnColor, 255);
	pixelRGBA(renderer, m.x, m.y-2, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x-1, m.x+1, m.y-1, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x-2, m.x+2, m.y, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x - 3, m.x - 1, m.y + 1, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x + 1, m.x + 3, m.y + 1, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x-3, m.x-2, m.y+2, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x+2, m.x+3, m.y+2, arrColor, arrColor, arrColor, 255);
	pixelRGBA(renderer, m.x-3, m.y+3, arrColor, arrColor, arrColor, 255);
	pixelRGBA(renderer, m.x+3, m.y+3, arrColor, arrColor, arrColor, 255);

	//Bottom
	arrColor = arrNone;
	btnColor = btnNone;
	if (ScrollBtnBHold) {
		arrColor = arrHold;
		btnColor = btnHold;
	}
	else if (ScrollBtnBHover) {
		arrColor = arrHover;
		btnColor = btnHover;
	}
	m.y = sbRect.h - ScrollBtnHeight + ScrollMidY;

	boxRGBA(renderer, sbRect.x + 1, sbRect.y + sbRect.h - ScrollBtnHeight, sbRect.x + ScrollWidth - 2, sbRect.y + sbRect.h - 1, btnColor, btnColor, btnColor, 255);
	pixelRGBA(renderer, m.x - 3, m.y - 2, arrColor, arrColor, arrColor, 255);
	pixelRGBA(renderer, m.x + 3, m.y - 2, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x - 3, m.x - 2, m.y - 1, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x + 2, m.x + 3, m.y - 1, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x - 3, m.x - 1, m.y, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x + 1, m.x + 3, m.y, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x - 2, m.x + 2, m.y + 1, arrColor, arrColor, arrColor, 255);
	hlineRGBA(renderer, m.x - 1, m.x + 1, m.y + 2, arrColor, arrColor, arrColor, 255);
	pixelRGBA(renderer, m.x, m.y + 3, arrColor, arrColor, arrColor, 255);

	//Bar
	int barNone = 205;
	int barAny = 192;
	int barHover = 166;
	int barDrag = 96;

	SDL_Rect bar = GetScrollBar_BarRect();
	
	int barColor;
	barColor = barNone;
	if (ScrollBarHover && !ScrollBarDrag) barColor = barHover;
	else if(ScrollBarDrag) barColor = barDrag;
	else if (ScrollBtnTHover || ScrollBtnBHover) barColor = barAny;

	boxRGBA(renderer, bar.x, bar.y, bar.x + bar.w - 1, bar.y + bar.h - 1, barColor, barColor, barColor, 255);
}

void CListBox::Step() {
	CWidget::Step();
	int mx, my;

	if (ScrollBarDrag) {
		SDL_GetGlobalMouseState(NULL, &my);
		int change = my - lastDragY;
		lastDragY = my;

		if (change != 0) {
			int offsetHeight = sbRect.h - (ScrollBtnHeight * 2);
			ScrollPos += (int)(change * 1.0 / offsetHeight * ScrollHeight);

			//ScrollPos += my - lastDragY;
			if (ScrollPos < 0) ScrollPos = 0;

			int maxHeight = ScrollHeight - GetListHeight();
			if (ScrollPos > maxHeight) ScrollPos = maxHeight;
		}
	}

	if (SDL_GetTicks() - LastStepMs < StepMs) return;
	
	if (ScrollBtnTHold || ScrollUpperHold) ScrollUp();

	if (ScrollBtnBHold || ScrollLowerHold) ScrollDown();

	LastStepMs = SDL_GetTicks();
}

void CListBox::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e);
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	if (e.type == SDL_MOUSEMOTION) {
		ScrollBtnTHover = false;
		ScrollBtnBHover = false;
		ScrollUpperHover = false;
		ScrollLowerHover = false;
		ScrollBarHover = false;
		if (MouseOver) {
			OnMouseMove(e);
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		LastStepMs = 0;
		if (MouseOver) {
			held = true;
			OnClick(e);
		}
		else {
			if (Focused) {
				if (CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
					SDL_Event event;
					SDL_zero(event);
					event.type = CUSTOMEVENT_WIDGET;
					event.window.windowID = e.window.windowID;
					event.user.code = WIDGET_FOCUS_LOST;
					event.user.data1 = this;
					event.user.data2 = Window;
					SDL_PushEvent(&event);
				}
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		MouseDown = false;
		if (ItemDrag) {
			ItemDrag = false;
			OnItemDragEnd(e);
		}
		ScrollBtnTHold = false;
		ScrollBtnBHold = false;
		ScrollUpperHold = false;
		ScrollLowerHold = false;
		ScrollBarDrag = false;
	}

	if (e.type == SDL_MOUSEWHEEL && Focused) {
		if (e.wheel.y > 0) // scroll up
		{
			for (int i = 0; i < e.wheel.y; i++) ScrollUp();
		}
		else if (e.wheel.y < 0) // scroll down
		{
			for (int i = 0; i < abs(e.wheel.y); i++) ScrollDown();
		}
	}
}

void CListBox::OnMouseMove(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	SDL_Rect inter = sbRect;
	inter.x += X + 3;
	inter.y += Y + captionOffset + 3 + 3;

	//Buttons
	SDL_Rect btn = inter;
	btn.w = ScrollWidth;
	btn.h = ScrollBtnHeight;
	if (doesPointIntersect(btn, mx, my)) {
		ScrollBtnTHover = true;
	}
	else {
		btn.y += inter.h - ScrollBtnHeight;
		if (doesPointIntersect(btn, mx, my)) {
			ScrollBtnBHover = true;
		}
	}

	//Bar
	SDL_Rect bar = GetScrollBar_BarRect();
	bar.x += X + 3;
	bar.y += Y + captionOffset + 3 + 3;
	if (doesPointIntersect(bar, mx, my)) {
		ScrollBarHover = true;
	}
	else {
		SDL_Rect scroll = sbRect;
		scroll.x += X + 3;
		scroll.y += Y + captionOffset + 3 + 3;
		scroll.y += ScrollBtnHeight;
		scroll.h -= ScrollBtnHeight;
		if (doesPointIntersect(scroll, mx, my)) {
			if (my < bar.y) ScrollUpperHover = true;
			if (my >= bar.y + bar.h) ScrollLowerHover = true;
		}
	}

	//Items
	SDL_Rect listRect = { X + 3, Y + captionOffset + 3 + 3, GetListWidth(), GetListHeight() };
	SDL_Point pos = { 0,0 };
	MouseOverItem = -1;
	for (int i = 0; i < Items.size(); i++) {
		SDL_Rect itemRect = { listRect.x + Items[i]->GetX(), listRect.y + pos.y, Items[i]->GetWidth(), Items[i]->GetHeight() };
		if (doesPointIntersect(itemRect, mx, my)) {
			MouseOverItem = i;
			if (MouseOverPoint.x != mx || MouseOverPoint.y != my) {
				MouseOverPoint = { mx, my };
				MouseOverStart = SDL_GetTicks();
			}
			break;
		}
		pos.y += Items[i]->GetHeight();
	}

	if (MouseDown && !ItemDrag && MouseOverItem >= 0 && (abs(mx - ClickPoint.x) >= 5 || abs(my - ClickPoint.y) >= 5)) {
		ItemDrag = true;
		OnItemDragStart(e);
		DragIndex = MouseOverItem;
	}
}

void CListBox::OnHoverStart(SDL_Event &e) {
	CWidget::OnHoverStart(e);
	if (MouseOverItem >= 0) OnListItemHoverStart(e);
}

void CListBox::OnHoverEnd(SDL_Event &e) {
	CWidget::OnHoverEnd(e);
	if (HoveringListItem) OnListItemHoverEnd(e);
}

void CListBox::OnListItemHoverStart(SDL_Event& e) {
	HoveringListItem = true;
	auto iter = eventMap.find("OnListItemHoverStart");
	if (iter != eventMap.end()) iter->second(e);
}

void CListBox::OnListItemHoverEnd(SDL_Event& e) {
	HoveringListItem = false;
	auto iter = eventMap.find("OnListItemHoverEnd");
	if (iter != eventMap.end()) iter->second(e);
}

/*void CListBox::OnListItemHover(SDL_Event& e) {
	auto iter = eventMap.find("OnListItemHover");
	if (iter != eventMap.end()) iter->second(e);
}*/

void CListBox::OnClick(SDL_Event& e) {
	MouseDown = true;
	auto iter = eventMap.find("Click");
	if (iter != eventMap.end()) iter->second(e);

	if (!Focused) {
		if (CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
			SDL_Event event;
			SDL_zero(event);
			event.type = CUSTOMEVENT_WIDGET;
			event.window.windowID = e.window.windowID;
			event.user.code = WIDGET_FOCUS_GAINED;
			event.user.data1 = this;
			event.user.data2 = Window;
			SDL_PushEvent(&event);
		}
	}

	//Scroll bar clicks should not click the list
	if (ScrollBtnTHover) {
		ScrollBtnTHold = true;
		return;
	}
	if (ScrollBtnBHover) {
		ScrollBtnBHold = true;
		return;
	}
	if (ScrollUpperHover) {
		ScrollUpperHold = true;
		return;
	}
	if (ScrollLowerHover) {
		ScrollLowerHold = true;
		return;
	}

	if (ScrollBarHover) {
		ScrollBarDrag = true;
		SDL_GetGlobalMouseState(NULL, &lastDragY);
		return;
	}

	//Widget
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	SDL_Rect listRect = { X + 3, Y + captionOffset + 3 + 3, GetListWidth(), GetListHeight() };
	if (!doesPointIntersect(listRect, mx, my)) return;
	
	//Items
	int px, py;
	px = mx - listRect.x;
	py = my - listRect.y;
	int clickIndex = -1;
	SDL_Point pos = { 0,0 };
	for (int i = 0; i < Items.size(); i++) {
		CImageBox *ib = Items[i];
		SDL_Rect itemRect = { pos.x, pos.y - ScrollPos, ib->GetWidth(), ib->GetHeight() };
		if (doesPointIntersect(itemRect, px, py)) {
			clickIndex = i;
			ClickPoint = {mx, my};
			break;
		}
		pos.y += ib->GetHeight();
	}

	if (Selected != clickIndex) {
		Selected = clickIndex;
		SelectionChange(e);
		SelectionClick(e);
	}
	else if (Selected == clickIndex && clickIndex >= 0) {
		if (e.button.clicks >= 2) SelectionDblClick(e);
		else  SelectionClick(e);
	}
}

void CListBox::SelectionChange(SDL_Event& e) {
	auto iter = eventMap.find("SelectionChange");
	if (iter != eventMap.end()) iter->second(e);
}

void CListBox::SelectionClick(SDL_Event& e) {
	auto iter = eventMap.find("SelectionClick");
	if (iter != eventMap.end()) iter->second(e);
}

void CListBox::SelectionDblClick(SDL_Event& e) {
	auto iter = eventMap.find("SelectionDblClick");
	if (iter != eventMap.end()) iter->second(e);
}

void CListBox::OnItemDragStart(SDL_Event &e) {
	auto iter = eventMap.find("OnItemDragStart");
	if (iter != eventMap.end()) iter->second(e);
	std::cout << Name << " List item Drag Start" << std::endl;
}

void CListBox::OnItemDragEnd(SDL_Event &e) {
	auto iter = eventMap.find("OnItemDragEnd");
	if (iter != eventMap.end()) iter->second(e);
	std::cout << Name << " List item Drag End" << std::endl;
}

void CListBox::OnFocusLost() {
	SDL_Event e;
	SDL_zero(e);
	e.user.data1 = this;

	if (ItemDrag) {
		ItemDrag = false;
		OnItemDragEnd(e);
	}
	if (HoveringListItem) {
		OnListItemHoverEnd(e);
	}
}

void CListBox::CreateListBoxTexture() {
	if (ListBoxTexture) SDL_DestroyTexture(ListBoxTexture);

	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);

	backColor = gui->backColor;
	SDL_Color white{ 255, 255, 255, 255 };
	SDL_Color dark{ 160, 160, 160, 255 };

	ListBoxTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, ListBoxTexture);
	SDL_SetRenderDrawColor(renderer, backColor.r, backColor.g, backColor.b, backColor.a);
	SDL_RenderClear(renderer);

	if (Text.length() > 0) {
		//Header, draw complex border
		rectangleRGBA(renderer, 1, captionOffset + 1, Width, Height, white.r, white.g, white.b, white.a);
		rectangleRGBA(renderer, 0, captionOffset, Width - 1, Height - 1, dark.r, dark.g, dark.b, dark.a);

		RenderText();

		fontRect.x = 5;
		fontRect.y = 1;
		boxRGBA(renderer, fontRect.x, fontRect.y, fontRect.w + 4, fontRect.h, backColor.r, backColor.g, backColor.b, backColor.a);
		SDL_RenderCopy(renderer, fontTexture, NULL, &fontRect);
	}
	else {
		//No header, draw simple border
		hlineRGBA(renderer, 0, Width - 1, 0, white.r, white.g, white.b, white.a);
		vlineRGBA(renderer, 0, 0, Height - 1, white.r, white.g, white.b, white.a);

		hlineRGBA(renderer, 0, Width - 1, Height - 1, dark.r, dark.g, dark.b, dark.a);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, dark.r, dark.g, dark.b, dark.a);
	}

	SDL_SetRenderTarget(renderer, priorTarget);
}

void CListBox::ScrollUp() {
	if (ScrollPos == 0) return;

	SDL_Rect listRect = { 0, ScrollPos, GetListWidth(), GetListHeight() };
	SDL_Rect lastRect;
	SDL_Point pos = { 0,0 };

	int lastY = 0;
	for (int i = 0; i < Items.size(); i++) {
		CImageBox *ib = Items[i];
		SDL_Rect itemRect = { pos.x, pos.y, ib->GetWidth(), ib->GetHeight() };
		if (doRectIntersect(listRect, itemRect)) {
			if (itemRect.y < ScrollPos) lastRect.y = itemRect.y;
			break;
		}
		lastRect = itemRect;
		pos.y += ib->GetHeight();
	}
	ScrollPos = lastRect.y;

	if (ScrollPos < 0) ScrollPos = 0;
}

void CListBox::ScrollDown() {
	int maxHeight = ScrollHeight - GetListHeight();
	if (ScrollPos >= maxHeight) return;

	SDL_Rect listRect = { 0, ScrollPos, GetListWidth(), GetListHeight() };
	SDL_Rect lastRect;
	SDL_Point pos = { 0,0 };

	int lastY = 0;
	bool intersectFound = false, breakOnNext = false;
	for (int i = 0; i < Items.size(); i++) {
		CImageBox *ib = Items[i];
		SDL_Rect lastRect = { pos.x, pos.y, ib->GetWidth(), ib->GetHeight() };
		if (breakOnNext) {
			break;
		}
		if (doRectIntersect(listRect, lastRect)) {
			breakOnNext = true;
		}
		pos.y += ib->GetHeight();
	}
	ScrollPos = pos.y;

	if (ScrollPos > maxHeight) ScrollPos = maxHeight;
}

int CListBox::GetListWidth() {
	return (Width - 6);
}

int CListBox::GetListHeight() {
	return (Height - captionOffset - 3 - 6);
}

int CListBox::GetItemsHeight() {
	int itemHeight = 0;
	for (auto item : Items) itemHeight += item->GetHeight();
	return itemHeight;
}

SDL_Rect CListBox::GetScrollBar_BarRect() {
	int offsetHeight = sbRect.h - (ScrollBtnHeight*2);
	int barHeight = (int)(((double)GetListHeight() / ScrollHeight) * offsetHeight) + 1;
	int barPos = (int)((((double)ScrollPos / ScrollHeight) * offsetHeight));
	SDL_Rect bar = { sbRect.x + 1, sbRect.y + ScrollBtnHeight + barPos, ScrollWidth - 2, barHeight };
	return bar;
}

void CListBox::AddItem(std::string text) {
	Texture *itemTexture = stringToTexture(renderer, text, gui->font, 0, gui->fontColor, 0);

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GetListWidth(), itemTexture->height);
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, gui->backColor.a);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, itemTexture->getTexture(), NULL, &itemTexture->getRect());

	SDL_SetRenderTarget(renderer, priorTarget);

	SDL_DestroyTexture(itemTexture->getTexture());
	itemTexture->texture = texture;
	itemTexture->width = GetListWidth();

	std::string ibName = Name + "_imageBox" + std::to_string(Items.size());
	CImageBox* imgItem = new CImageBox(Window, Name, 0, 0);
	imgItem->SetWidth(itemTexture->width);
	imgItem->SetHeight(itemTexture->height);
	imgItem->SetAnchor(ANCHOR_TOPLEFT);
	imgItem->SetImage(itemTexture);
	AddItem(imgItem);
}

void CListBox::AddItem(CImageBox* item) {
	Items.push_back(item);

	ScrollHeight = GetItemsHeight();
	if (ScrollHeight <= GetListHeight()) {
		UseScroll = false;
		return;
	}

	UseScroll = true;
	sbRect.x = GetListWidth() - ScrollWidth;
	sbRect.y = 0;
	sbRect.w = ScrollWidth;
	sbRect.h = GetListHeight();
}

void CListBox::SetSelectedIndex(int index) {
	Selected = index;
	if (Selected < 0) Selected = -1;
	if (Selected > Items.size()) Selected = -1;
}