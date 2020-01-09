#include "stdafx.h"
#include "Label.h"

CLabel::CLabel(CWindow* window, std::string name, int x, int y) : CWidget(window, name, x, y) {
	Name = name;
	X = x;
	Y = y;
}

CLabel::CLabel(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("Text")) SetText(vWidget["Text"].GetString());
	if (vWidget.HasMember("Alignment")) alignment = (LabelAlignment)vWidget["Alignment"].GetInt();
	if (vWidget.HasMember("VAlignment")) valignment = (LabelVAlignment)vWidget["VAlignment"].GetInt();
	if (vWidget.HasMember("Underlined")) Underlined = vWidget["Underlined"].GetBool();
	if (vWidget.HasMember("Multiline")) Multiline = vWidget["Multiline"].GetBool();

	if (Multiline) TextWrapWidth = Width;
}

CLabel::~CLabel() {
	//
}

void CLabel::ReloadAssets() {
	if (fontTexture) {
		SDL_DestroyTexture(fontTexture);
		fontTexture = NULL;
		RenderText();
	}
}

void CLabel::Render() {
	if (!Visible) return;
	if (!fontTexture) RenderText();

	labelRect = fontRect;
	switch (alignment) {
	case laLeft:
		labelRect.x = X;
		break;
	case laRight:
		labelRect.x = X + Width - fontRect.w;
		break;
	case laCenter:
		labelRect.x = X + (Width / 2) - (fontRect.w / 2);
		break;
	}
	switch (valignment) {
	case lvaTop:
		labelRect.y = Y;
		break;
	case lvaBottom:
		labelRect.y = Y + Height - fontRect.h;
		break;
	case lvaCenter:
		labelRect.y = Y + (Height / 2) - (fontRect.h / 2);
		break;
	}

	SDL_RenderCopy(renderer, fontTexture, NULL, &labelRect);
}

void CLabel::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(widgetRect, mx, my)) {
			held = true;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(widgetRect, mx, my)) {
			if (held) {
				OnClick(e);
			}

		}
		held = false;
	}
}

void CLabel::OnClick(SDL_Event& e) {
	auto iter = eventMap.find("Click");
	if (iter != eventMap.end()) iter->second(e);
}

void CLabel::SetWrapLength(int len) {
	TextWrapWidth = len;
}

void CLabel::SetBold(bool bold) {
	Bold = bold;
	RenderText();
}

void CLabel::SetAlignment(LabelAlignment align) {
	alignment = align;
}

void CLabel::SetVAlignment(LabelVAlignment valign) {
	valignment = valign;
}

SDL_Rect CLabel::GetTextRect() {
	return fontRect;
}

// Start old Label, to be deprecated

Label::Label(std::string sLine, int toX, int toY, bool isBold) : Widget(toX, toY) {
	bold = isBold;
	loadFont(bold);
	setText(sLine);

	width = fontRect.w;
	height = fontRect.h;

	alignment = laLeft;
	initTextRect();
}


Label::~Label() {
}


void Label::render() {
	if (!visible) return;
	SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);
}


void Label::offsetPosition(SDL_Point offset) {
	Widget::offsetPosition(offset);
	initTextRect();
}


void Label::offsetPosition(int offsetX, int offsetY) {
	Widget::offsetPosition(offsetX, offsetY);
	initTextRect();
}


void Label::setAlignment(LabelAlignment algn) {
	alignment = algn;
	initTextRect();
}


void Label::initTextRect() {
	switch (alignment) {
	case laLeft:
		textRect = { x, y, fontRect.w, fontRect.h };
		break;
	case laCenter:
		textRect = { x - (fontRect.w/2), y, fontRect.w, fontRect.h };
		break;
	case laRight:
		textRect = { x - fontRect.w, y, fontRect.w, fontRect.h };
		break;
	default:
		textRect = { x, y, fontRect.w, fontRect.h };
		break;
	}
	
}


void Label::setPosition(int px, int py) {
	Widget::setPosition(px, py);
	initTextRect();
}


void Label::setText(std::string sText) {
	Widget::setText(sText);
	initTextRect();
}


void Label::renderText(std::string sText) {
	Widget::renderText(sText);
	initTextRect();
}

int Label::getHeight() {
	return textRect.h;
}