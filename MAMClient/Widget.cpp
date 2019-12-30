#include "stdafx.h"
#include "Widget.h"
#include "Window.h"

Uint32 CUSTOMEVENT_WIDGET = SDL_RegisterEvents(1);

CWidget::CWidget(CWindow* window) {
	Window = window;
	renderer = Window->renderer;
	font = gui->font;
	fontColor = gui->fontColor;
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

	font = gui->font;
	fontColor = gui->fontColor;
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
	if (vWidget.HasMember("ReadOnly")) ReadOnly = vWidget["ReadOnly"].GetBool();

	widgetRect = SDL_Rect{ X, Y, Width, Height };
	font = gui->font;
	fontColor = gui->fontColor; 
	backColor = { 0, 0, 0, 0 };
	Loaded = true;
}


CWidget::~CWidget() {
	if (fontTexture) SDL_DestroyTexture(fontTexture);
}

/*void CWidget::SetRenderer(SDL_Renderer* rend) {
	renderer = rend;
}*/

void CWidget::SetText(std::string value) {
	Text = value;
	RenderText();
}

void CWidget::RenderText() {
	if (!renderer) return;
	if (!Loaded) return;
	if (fontTexture) SDL_DestroyTexture(fontTexture);
	SDL_Surface* lSurface;

	int fontStyle = TTF_STYLE_NORMAL;
	if (Underlined) fontStyle |= TTF_STYLE_UNDERLINE;
	if (Bold) fontStyle |= TTF_STYLE_BOLD;

	TTF_SetFontStyle(font, fontStyle);

	if (TextWrapWidth > 0) lSurface = TTF_RenderText_Blended_Wrapped(font, Text.c_str(), fontColor, TextWrapWidth);
	else lSurface = TTF_RenderText_Blended(font, Text.c_str(), fontColor);

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	if (lSurface != NULL) {
		fontRect = { 0, 0, lSurface->w, lSurface->h };
		fontTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, fontRect.w, fontRect.h);

		SDL_Texture* priorTarget = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, fontTexture);
		SDL_SetRenderDrawColor(renderer, backColor.r, backColor.g, backColor.b, backColor.a);
		SDL_RenderClear(renderer);
		SDL_SetTextureBlendMode(fontTexture, SDL_BLENDMODE_BLEND);
		SDL_Texture *tempTexture = SDL_CreateTextureFromSurface(renderer, lSurface);
		SDL_RenderCopy(renderer, tempTexture, NULL, NULL);
		SDL_RenderCopy(renderer, tempTexture, NULL, NULL);
		SDL_SetRenderTarget(renderer, priorTarget);

		SDL_DestroyTexture(tempTexture);
		SDL_FreeSurface(lSurface);
	}
}

void CWidget::Render() {
	//
}

void CWidget::HandleEvent(SDL_Event& e) {
	//
}

bool CWidget::DoesPointIntersect(SDL_Point point) {
	if (point.x >= X && point.x <= X + Width && point.y >= Y && point.y <= Y + Height) return true;
	return false;
}


void CWidget::RegisterEvent(std::string eventName, EventFunc evf) {
	eventMap[eventName] = evf;
}

void CWidget::OnFocus() {
	Focused = true;
}

void CWidget::OnFocusLost() {
	Focused = false;
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

void CWidget::SetFontColor(SDL_Color fc) {
	fontColor = fc;
	RenderText();
}

void CWidget::SetBackColor(SDL_Color bc) {
	backColor = bc;
	RenderText();
}

CWidget* CWidget::GetParent() {
	return Parent;
}

int CWidget::GetTabItem() {
	return TabItem;
}


/* --------------------------- Start Old Widget Class Below - To be Replaced ---------------------------------- */

Widget::Widget(int toX, int toY) {
	renderer = gClient.renderer;
	x = toX;
	y = toY;
	visible = true;
	fontColor = gui->fontColor;
}

Widget::~Widget() {
	SDL_DestroyTexture(wTexture); wTexture = NULL;
	SDL_DestroyTexture(fontTexture); fontTexture = NULL;
}


bool operator<(const Widget &s1, const Widget &s2) {
	return s1.priority > s2.priority;
}

void Widget::render() {
	if (!visible) return;
}

bool Widget::handleEvent(SDL_Event* e) {
	return false;
}

void Widget::loadFont() {
	loadFont(false);
}

void Widget::loadFont(bool isBold) {
	font = gui->font;
	/*if (!isBold) font = gui->font;
	else font = gui->font_bold;*/
}

void Widget::setText(std::string sText) {
	text = sText;
	renderText(text);
}


void Widget::setTextWidth(int tWidth) {
	textWidth = tWidth;
	renderText(text);
}


void Widget::renderText(std::string sText) {
	SDL_Surface* lSurface;
	if (fontTexture) SDL_DestroyTexture(fontTexture);

	int fontStyle = TTF_STYLE_NORMAL;
	if (underlined) fontStyle |= TTF_STYLE_UNDERLINE;
	if (bold) fontStyle |= TTF_STYLE_BOLD;

	//if (underlined) TTF_SetFontStyle(font, TTF_STYLE_UNDERLINE);
	TTF_SetFontStyle(font, fontStyle);

	if (textWidth > 0) lSurface = TTF_RenderText_Blended_Wrapped(font, sText.c_str(), fontColor, textWidth);
	else lSurface = TTF_RenderText_Blended(font, sText.c_str(), fontColor);

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	if (lSurface != NULL) {
		fontTexture = SDL_CreateTextureFromSurface(renderer, lSurface);
		fontRect = { 0, 0, lSurface->w, lSurface->h };

		SDL_FreeSurface(lSurface);
	}
}

void Widget::setVisibility(bool setVisible) {
	visible = setVisible;
}


void Widget::setFont(TTF_Font* newFont) {
	font = newFont;
	renderText(text);
}

void Widget::setFontColor(SDL_Color aColor) {
	fontColor = aColor;
	renderText(text);
}


void Widget::setPosition(SDL_Point pos) {
	x = pos.x;
	y = pos.y;
}


void Widget::setPosition(int px, int py) {
	x = px;
	y = py;
}


void Widget::offsetPosition(SDL_Point offset) {
	x += offset.x;
	y += offset.y;
}


void Widget::offsetPosition(int offsetX, int offsetY) {
	x += offsetX;
	y += offsetY;
}


void Widget::setPriority(int pri) {
	priority = pri;
}

void Widget::setDepth(int dep) {
	depth = dep;
}

void Widget::setUnderlinded(bool und) {
	underlined = und;
	renderText(text);
}