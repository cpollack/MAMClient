#include "stdafx.h"
#include "Widget.h"
#include "Window.h"

CWidget::CWidget(CWindow* window) {
	Window = window;
	renderer = Window->renderer;

	strFont = ARIAL;
	strFontUni = ARIALUNI;
	LoadFont();
	backColor = { 0, 0, 0, 0 };
	Loaded = true;
}


CWidget::CWidget(CWindow* window, std::string name, int x, int y) {
	Window = window;
	renderer = Window->renderer;
	Name = name;
	X = x;
	Y = y;
	widgetRect = SDL_Rect{ X, Y, Width, Height };

	strFont = ARIAL;
	strFontUni = ARIALUNI;
	LoadFont();
	backColor = { 0, 0, 0, 0 };
	Loaded = true;
}


CWidget::CWidget(CWindow* window, rapidjson::Value& vWidget) {
	Window = window;
	renderer = Window->renderer;
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("Name")) Name = vWidget["Name"].GetString();
	if (vWidget.HasMember("X")) X = vWidget["X"].GetInt();
	if (vWidget.HasMember("Y")) Y = vWidget["Y"].GetInt();
	if (vWidget.HasMember("Width")) Width = vWidget["Width"].GetInt();
	if (vWidget.HasMember("Height")) Height = vWidget["Height"].GetInt();
	if (vWidget.HasMember("TabItem")) TabItem = vWidget["TabItem"].GetInt();
	if (vWidget.HasMember("Depth")) Depth = vWidget["Depth"].GetInt();
	if (vWidget.HasMember("ReadOnly")) ReadOnly = vWidget["ReadOnly"].GetBool();

	widgetRect = SDL_Rect{ X, Y, Width, Height };
	strFont = ARIAL;
	strFontUni = ARIALUNI;
	LoadFont();
	backColor = { 0, 0, 0, 0 };
	Loaded = true;
}


CWidget::~CWidget() {
	if (fontTexture) SDL_DestroyTexture(fontTexture);
	if (font) TTF_CloseFont(font);
	if (fontUni) TTF_CloseFont(fontUni);
}

/*void CWidget::SetRenderer(SDL_Renderer* rend) {
	renderer = rend;
}*/

void CWidget::LoadFont() {
	if (font) {
		TTF_CloseFont(font);
		font = nullptr;
	}

	font = TTF_OpenFont(strFont.c_str(), fontSize);
	fontColor = gui->fontColor;
}

void CWidget::LoadFontUni() {
	if (fontUni) {
		TTF_CloseFont(fontUni);
		fontUni = nullptr;
	}

	fontUni = TTF_OpenFont(strFontUni.c_str(), fontSize);
	fontColor = gui->fontColor;
}

void CWidget::SetText(std::string value) {
	Text = value;
	wText.clear();
	if (!font) LoadFont();
	RenderText();
}

void CWidget::SetText(std::wstring value) {
	wText = value;
	Text.clear();
	if (!fontUni) LoadFontUni();
	RenderText();
}

void CWidget::RenderText() {
	if (!renderer) return;
	if (!Loaded) return;
	if (fontTexture) {
		SDL_DestroyTexture(fontTexture);
		fontTexture = NULL;
	}
	SDL_Surface* lSurface;

	int fontStyle = TTF_STYLE_NORMAL;
	if (Underlined) fontStyle |= TTF_STYLE_UNDERLINE;
	if (Bold) fontStyle |= TTF_STYLE_BOLD;

	if (wText.length()) {
		TTF_SetFontStyle(fontUni, fontStyle);
		std::wstring finalText = wText;
		if (finalText.length() == 0) finalText = L" ";
		if (TextWrapWidth > 0) lSurface = TTF_RenderUNICODE_Blended_Wrapped(fontUni, (const Uint16*)finalText.c_str(), fontColor, TextWrapWidth);
		else lSurface = TTF_RenderUNICODE_Blended(fontUni, (const Uint16*)finalText.c_str(), fontColor);
		TTF_SetFontStyle(fontUni, TTF_STYLE_NORMAL);
	}
	else {
		TTF_SetFontStyle(font, fontStyle);
		std::string finalText = Text;
		if (finalText.length() == 0) finalText = " ";
		if (TextWrapWidth > 0) lSurface = TTF_RenderUTF8_Blended_Wrapped(font, finalText.c_str(), fontColor, TextWrapWidth);
		else lSurface = TTF_RenderUTF8_Blended(font, finalText.c_str(), fontColor);
		TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
	}

	if (lSurface != NULL) {
		fontRect = { 0, 0, Width, lSurface->h };
		if (lSurface->w < Width) fontRect.w = lSurface->w;
		fontTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, fontRect.w, fontRect.h);

		SDL_Texture* priorTarget = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, fontTexture);
		if (UseBackground) {
			SDL_SetRenderDrawColor(renderer, backColor.r, backColor.g, backColor.b, backColor.a);
			SDL_RenderClear(renderer);
		}
		SDL_SetTextureBlendMode(fontTexture, SDL_BLENDMODE_BLEND);
		SDL_Texture *tempTexture = SDL_CreateTextureFromSurface(renderer, lSurface);

		// Show rightmost side of available text within width, does not take into account cursor/scroll position 
		SDL_Rect srcRect = { 0, 0, lSurface->w, lSurface->h };
		if (srcRect.w > Width) {
			srcRect.x = srcRect.w - Width;
			srcRect.w = Width;
		}
		//Only shop topmost part of text
		/*if (srcRect.h > Height) {
			srcRect.h = Height;
		}*/

		SDL_RenderCopy(renderer, tempTexture, &srcRect, &fontRect);
		SDL_RenderCopy(renderer, tempTexture, &srcRect, &fontRect);
		SDL_SetRenderTarget(renderer, priorTarget);

		SDL_DestroyTexture(tempTexture);
		SDL_FreeSurface(lSurface);
	}
}

void CWidget::Render() {
	//
}

void CWidget::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEMOTION) {
		MouseOver = false;
		if (Visible && doesPointIntersect(SDL_Rect{ X,Y,Width,Height }, SDL_Point{ e.motion.x, e.motion.y })) {
			MouseOver = true;
			if (MouseOverStart == 0) MouseOverStart = SDL_GetTicks();
			if (HoverPoint.x != e.motion.x || HoverPoint.y != e.motion.y) {
				if (Hovering) {
					Hovering = false;
					OnHoverEnd(e);
				}
				HoverStart = SDL_GetTicks();
			}
			HoverPoint = { e.motion.x, e.motion.y };
		}
		else {
			MouseOverStart = 0;
			HoverStart = 0;
			if (Hovering) {
				Hovering = false;
				OnHoverEnd(e);
			}
		}
	}
}

void CWidget::Step() {
	if (!Hovering && HoverStart > 0 && SDL_GetTicks() - HoverStart >= HoverDelay) {
		Hovering = true;
		SDL_Event e;
		SDL_zero(e);
		e.user.data1 = this;
		OnHoverStart(e);
	}
}

bool CWidget::DoesPointIntersect(SDL_Point point) {
	if (point.x >= X && point.x <= X + Width && point.y >= Y && point.y <= Y + Height) return true;
	return false;
}


void CWidget::RegisterEvent(std::string eventName, EventFunc evf) {
	eventMap[eventName] = evf;
}

void CWidget::RegisterEvent(std::string widgetName, std::string eventName, EventFunc evf) {
	auto iter = widgets.find(widgetName);
	if (iter != widgets.end()) {
		if (iter->second) iter->second->RegisterEvent(eventName, evf);
	}
}

void CWidget::OnFocus() {
	Focused = true;
}

void CWidget::OnFocusLost() {
	Focused = false;
}

void CWidget::OnHoverStart(SDL_Event &e) {
	auto iter = eventMap.find("OnHoverStart");
	if (iter != eventMap.end()) iter->second(e);
	std::cout << Name << " Hover Start" << std::endl;
}

void CWidget::OnHoverEnd(SDL_Event &e) {
	auto iter = eventMap.find("OnHoverEnd");
	if (iter != eventMap.end()) iter->second(e);
	std::cout << Name << " Hover End" << std::endl;
}

void CWidget::SetFocus(bool focus) {
	Focused = focus;
}

bool CWidget::IsFocus() {
	return Focused;
}

void CWidget::SetX(int x) {
	X = x;
	widgetRect.x = X;
}

void CWidget::SetY(int y) {
	Y = y;
	widgetRect.y = Y;
}

void CWidget::SetPosition(int toX, int toY) {
	SetPosition(SDL_Point{ toX, toY });
}

void CWidget::SetPosition(SDL_Point p) {
	X = p.x;
	Y = p.y;
	widgetRect.x = X;
	widgetRect.y = Y;
}

void CWidget::SetWidth(int w) {
	Width = w;
	widgetRect.w = Width;
}

void CWidget::SetHeight(int h) {
	Height = h;
	widgetRect.h = Height;
}

void CWidget::SetWindow(CWindow* win) {
	Window = win;
}

void CWidget::SetParent(CWidget* widget) {
	Parent = widget;
}

void CWidget::AddChild(CWidget* widget) {
	Children.push_back(widget);
}

void CWidget::SetVisible(bool visible) {
	Visible = visible;
}

void CWidget::SetFont(std::string fontFile) {
	strFont = fontFile;
	LoadFont();
}

void CWidget::SetFontSize(int size) {
	fontSize = size;
	LoadFont();
}

void CWidget::SetFontColor(SDL_Color fc) {
	fontColor = fc;
	RenderText();
}

void CWidget::SetBackColor(SDL_Color bc) {
	backColor = bc;
	UseBackground = true;
	RenderText();
}

CWidget* CWidget::GetParent() {
	return Parent;
}

int CWidget::GetTabItem() {
	return TabItem;
}
