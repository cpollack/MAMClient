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
	//fontTexture freed in CWidget
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

	SDL_Rect srcRect = fontRect;
	SDL_Rect dstRect = widgetRect;

	//Offset Rendering positions
	switch (alignment) {
	case laLeft:
		//labelRect.x = X;
		break;
	case laRight:
		if (srcRect.w > dstRect.w) srcRect.x = dstRect.w - srcRect.w;
		else dstRect.x += (dstRect.w - srcRect.w);
		//labelRect.x = X + Width - fontRect.w;
		break;
	case laCenter:
		if (srcRect.w > dstRect.w) srcRect.x = (dstRect.w / 2) - (srcRect.w / 2);
		else dstRect.x -= (srcRect.w / 2) - (dstRect.w / 2);
		//labelRect.x = X + (Width / 2) - (fontRect.w / 2);
		break;
	}
	switch (valignment) {
	case lvaTop:
		//labelRect.y = Y;
		break;
	case lvaBottom:
		if (srcRect.h > dstRect.h) srcRect.y = dstRect.h - srcRect.h;
		else dstRect.y += (dstRect.h - srcRect.h);
		//labelRect.y = Y + Height - fontRect.h;
		break;
	case lvaCenter:
		if (srcRect.h > dstRect.h) srcRect.y = (dstRect.h / 2) - (srcRect.h / 2);
		else dstRect.y -= (srcRect.h / 2) - (dstRect.h / 2);
		//labelRect.y = Y + (Height / 2) - (fontRect.h / 2);
		break;
	}

	//Equalize rectangles
	if (srcRect.w > dstRect.w) srcRect.w = dstRect.w;
	if (srcRect.h > dstRect.h) srcRect.h = dstRect.h;
	if (dstRect.w > srcRect.w) dstRect.w = srcRect.w;
	if (dstRect.h > srcRect.h) dstRect.h = srcRect.h;

	SDL_RenderCopy(renderer, fontTexture, &srcRect, &dstRect);
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
