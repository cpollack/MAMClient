#include "stdafx.h"

#include "Window.h"
#include "Dropdown.h"
#include "CustomEvents.h"

CDropDown::CDropDown(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	SetPosition(SDL_Point{ x,y });

	//fontColor = gui->buttonFontColor;
}

CDropDown::CDropDown(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	//fontColor = gui->buttonFontColor;
	//if (vWidget.HasMember("Text")) SetText(vWidget["Text"].GetString());
	if (vWidget.HasMember("Detailed")) Detailed = vWidget["Detailed"].GetBool();
}

CDropDown::~CDropDown() {
	if (dropdownTexture) SDL_DestroyTexture(dropdownTexture);
	if (expandTexture) SDL_DestroyTexture(expandTexture);
	if (arrowDownTexture) SDL_DestroyTexture(arrowDownTexture);
	if (arrowUpTexture) SDL_DestroyTexture(arrowUpTexture);
}

void CDropDown::Render() {
	if (!Visible) return;
	if (!dropdownTexture) CreateDropdownTexture();

	SDL_RenderCopy(renderer, dropdownTexture, NULL, &widgetRect);
	SDL_RenderCopy(renderer, arrowTexture, NULL, &arrowRect);

	//Render Text
	if (fontTexture) {
		SDL_Rect textRect = fontRect;
		textRect.x = X + 2;
		textRect.y = Y + (Height / 2) - (textRect.h / 2);
		SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);
	}

	if (Focused && expandTexture) {
		int rowSize = TTF_FontHeight(gui->font) + DD_ROWSPACER;
		SDL_RenderCopy(renderer, expandTexture, NULL, &expandRect);
		for (int i = 0; i < rows.size(); i++) {
			SDL_Rect rowRect = { expandRect.x + 2, expandRect.y + (rowSize * i), Width - 6, rowSize};
			rows[i].render(rowRect);
		}
	}
}

void CDropDown::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEMOTION) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		SDL_Rect moveRect = widgetRect;
		if (expanded) {
			moveRect.h += expandRect.h;
			if (expandRect.y < moveRect.y) moveRect.y = expandRect.y;
		}
		if (doesPointIntersect(moveRect, mx, my)) {
			mouseOver = true;
			OnMouseMove(e);
		}
		else {
			mouseOver = false;
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		SDL_Rect downRect = widgetRect;
		if (expanded) {
			downRect.h += expandRect.h;
			if (expandRect.y < downRect.y) downRect.y = expandRect.y;
		}
		if (doesPointIntersect(downRect, mx, my)) {
			held = true;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		SDL_Rect upRect = widgetRect;
		if (expanded) {
			upRect.h += expandRect.h;
			if (expandRect.y < upRect.y) upRect.y = expandRect.y;
		}
		if (doesPointIntersect(upRect, mx, my)) {
			if (held) {
				pressed = !pressed;
				OnClick(e);
			}

		}
		else {
			//fire focus lost event
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
			expanded = false;
		}
		held = false;
	}
}

void CDropDown::SetFocus(bool focus) {
	/*bool priorFocus = Focused;
	CWidget::SetFocus(focus);
	if (priorFocus != Focused) {
		if (Focused) OnFocus();
		else OnFocusLost();
	}*/
}

void CDropDown::OnFocus() {
	arrowTexture = arrowUpTexture;
	Focused = true;
}


void CDropDown::OnFocusLost() {
	arrowTexture = arrowDownTexture;
	Focused = false;
}

void CDropDown::OnMouseMove(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	if (expanded) {
		if (doesPointIntersect(expandRect, mx, my)) {
			int yPos = my - expandRect.y;
			int rowSize = TTF_FontHeight(gui->font) + DD_ROWSPACER;
			for (int i = 0; i < rows.size(); i++) {
				if (i * rowSize <= yPos && (i * rowSize) + rowSize > yPos) {
					rows[i].setFocused(true);
				}
				else rows[i].setFocused(false);
			}
		}
	}

	auto iter = eventMap.find("MouseMove");
	if (iter != eventMap.end()) iter->second(e);
}

void CDropDown::OnClick(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	if (expanded) {
		if (doesPointIntersect(expandRect, mx, my)) {
			int yPos = my - expandRect.y;
			int rowSize = TTF_FontHeight(gui->font) + DD_ROWSPACER;
			for (int i = 0; i < rows.size(); i++) {
				if (i * rowSize <= yPos && (i * rowSize) + rowSize >= yPos) {
					SetValue(i);
					break;
				}
			}
		}
		//fire focus lost event
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
		expanded = false;
	}
	else {
		//fire focus event
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
		expanded = !expanded;
	}

	auto iter = eventMap.find("Click");
	if (iter != eventMap.end()) iter->second(e);
}

void CDropDown::OnChange(SDL_Event& e) {
	auto iter = eventMap.find("Change");
	if (iter != eventMap.end()) iter->second(e);
}

void CDropDown::CreateDropdownTexture() {
	if (dropdownTexture) SDL_DestroyTexture(dropdownTexture);
	if (arrowDownTexture) SDL_DestroyTexture(arrowDownTexture);
	if (arrowUpTexture) SDL_DestroyTexture(arrowUpTexture);

	dropdownTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, dropdownTexture);
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, gui->backColor.a);
	SDL_RenderClear(renderer);

	SDL_Color black = { 0, 0, 0, 255 };
	SDL_Color dark2 = { 105, 105, 105, 255 };
	SDL_Color dark1 = { 160, 160, 160, 255 };
	SDL_Color light1 = { 227, 227, 227, 255 };
	SDL_Color light2 = { 240, 240, 240, 255 };
	SDL_Color white = { 255, 255, 255, 255 };

	if (Detailed) {
		hlineRGBA(renderer, 0, Width - 1, 0, dark1.r, dark1.g, dark1.b, dark1.a);
		vlineRGBA(renderer, 0, 0, Height - 1, dark1.r, dark1.g, dark1.b, dark1.a);
		hlineRGBA(renderer, 1, Width - 2, 1, dark2.r, dark2.g, dark2.b, dark2.a);
		vlineRGBA(renderer, 1, 1, Height - 2, dark2.r, dark2.g, dark2.b, dark2.a);

		hlineRGBA(renderer, 0, Width - 1, Height - 1, white.r, white.g, white.b, white.a);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, white.r, white.g, white.b, white.a);
		hlineRGBA(renderer, 1, Width - 2, Height - 2, light2.r, light2.g, light2.b, light2.a);
		vlineRGBA(renderer, Width - 2, 1, Height - 2, light2.r, light2.g, light2.b, light2.a);
	}
	else {
		rectangleRGBA(renderer, 0, 0, Width - 1, Height - 1, black.r, black.g, black.b, black.a);
		rectangleRGBA(renderer, Width - 13, 0, Width - 1, Height - 1, black.r, black.g, black.b, black.a);
	}	

	SDL_SetRenderTarget(renderer, NULL);

	int rectWidth = Height - 4;
	int rectHeight = Height - 4;
	arrowRect = { X + Width - rectHeight - 2, Y + 2, rectWidth, rectHeight };
	SDL_Point center = { rectWidth / 2, rectHeight / 2 };

	arrowDownTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rectWidth, rectHeight);
	arrowUpTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rectWidth, rectHeight);
	SDL_SetRenderTarget(renderer, arrowDownTexture);

	if (Detailed) SDL_SetRenderDrawColor(renderer, light1.r, light1.g, light1.b, light1.a);
	else SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, gui->backColor.a);
	SDL_RenderClear(renderer);

	if (Detailed) {
		hlineRGBA(renderer, 0, rectWidth - 1, 0, light2.r, light2.g, light2.b, light2.a);
		vlineRGBA(renderer, 0, 0, rectHeight - 1, light2.r, light2.g, light2.b, light2.a);
		hlineRGBA(renderer, 1, rectWidth - 2, 1, white.r, white.g, white.b, white.a);
		vlineRGBA(renderer, 1, 1, rectHeight - 2, white.r, white.g, white.b, white.a);

		hlineRGBA(renderer, 0, rectWidth - 1, rectHeight - 1, dark2.r, dark2.g, dark2.b, dark2.a);
		vlineRGBA(renderer, rectWidth - 1, 0, rectHeight - 1, dark2.r, dark2.g, dark2.b, dark2.a);
		hlineRGBA(renderer, 1, rectWidth - 2, rectHeight - 2, dark1.r, dark1.g, dark1.b, dark1.a);
		vlineRGBA(renderer, rectWidth - 2, 1, rectHeight - 2, dark1.r, dark1.g, dark1.b, dark1.a);
	}
	else {
		rectangleRGBA(renderer, 0, 0, rectWidth - 1, rectHeight - 1, black.r, black.g, black.b, black.a);
	}

	SDL_SetRenderTarget(renderer, arrowUpTexture);
	SDL_RenderCopy(renderer, arrowDownTexture, NULL, NULL);

	hlineRGBA(renderer, center.x - 3, center.x + 3, center.y + 2, black.r, black.g, black.b, black.a);
	hlineRGBA(renderer, center.x - 2, center.x + 2, center.y + 1, black.r, black.g, black.b, black.a);
	hlineRGBA(renderer, center.x - 1, center.x + 1, center.y, black.r, black.g, black.b, black.a);
	vlineRGBA(renderer, center.x, center.y, center.y - 1, black.r, black.g, black.b, black.a);

	SDL_SetRenderTarget(renderer, arrowDownTexture);

	hlineRGBA(renderer, center.x - 3, center.x + 3, center.y - 1, black.r, black.g, black.b, black.a);
	hlineRGBA(renderer, center.x - 2, center.x + 2, center.y, black.r, black.g, black.b, black.a);
	hlineRGBA(renderer, center.x - 1, center.x + 1, center.y + 1, black.r, black.g, black.b, black.a);
	vlineRGBA(renderer, center.x, center.y + 1, center.y + 2, black.r, black.g, black.b, black.a);

	SDL_SetRenderTarget(renderer, NULL);

	if (expanded) arrowTexture = arrowUpTexture;
	else arrowTexture = arrowDownTexture;

	if (!fontTexture) RenderText();
}

void CDropDown::AddRow(std::string row) {
	rows.push_back(ddRow(renderer, row));

	if (expandTexture) SDL_DestroyTexture(expandTexture);

	expandRect.x = X;
	expandRect.y = Y + Height;
	expandRect.w = Width;
	int rowHeight = TTF_FontHeight(gui->font) + DD_ROWSPACER;
	expandRect.h = rowHeight * rows.size();

	if (expandRect.y + expandRect.h > Window->GetHeight()) {
		expandRect.y = Y - expandRect.h;
	}

	expandTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, expandRect.h);
	SDL_SetRenderTarget(renderer, expandTexture);
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, gui->backColor.a);
	SDL_RenderClear(renderer);
	rectangleRGBA(renderer, 0, 0, expandRect.w - 1, expandRect.h - 1, 0x64, 0x64, 0x64, 0xFF);
	SDL_SetRenderTarget(renderer, NULL);
}

void CDropDown::SetValue(int row) {
	if (selectedRow == row) return;

	selectedRow = row;
	rows[row].setFocused(true);
	SetText(rows[row].text);
	SDL_Event e;
	OnChange(e);
}

ddRow::ddRow(SDL_Renderer* renderer, std::string txt) {
	this->renderer = renderer;
	setText(txt);
}

void ddRow::setText(std::string txt) {
	text = txt;
}

void ddRow::setFocused(bool foc) {
	if (focused != foc) {
		focused = foc;
		//setText(text);
	}
}

void ddRow::render(SDL_Rect renderRect) {
	if (focused) {
		boxRGBA(renderer, renderRect.x, renderRect.y, renderRect.x + renderRect.w, renderRect.y + renderRect.h, 0x33, 0x99, 0xFF, 0xFF);
	}
	SDL_Rect toRect = renderRect;

	if (texture) SDL_DestroyTexture(texture);
	TTF_SetFontStyle(gui->font, TTF_STYLE_NORMAL);
	SDL_Color color = gui->fontColor;
	if (focused) color = { 255, 255, 255, 255 };

	SDL_Surface* lSurface = TTF_RenderText_Blended(gui->font, text.c_str(), color);
	if (lSurface != NULL) {
		rect = { 0, 0, lSurface->w, lSurface->h };
		texture = SDL_CreateTextureFromSurface(renderer, lSurface);
		SDL_FreeSurface(lSurface);
	}

	if (toRect.w > rect.w) toRect.w = rect.w;
	toRect.h -= DD_ROWSPACER;
	toRect.y += (DD_ROWSPACER / 2);
	SDL_RenderCopy(renderer, texture, NULL, &toRect);
}
