#include "stdafx.h"
#include "Text.h"
#include "Define.h"

#include "GUI.h"

Text::Text(SDL_Renderer* r, std::string sLine, int toX, int toY, bool isBold) {
	renderer = r;
	rect.x = toX;
	rect.y = toY;

	bold = isBold;
	font = gui->font;
	color = { 255, 255, 255, 255 };
	setText(sLine);

	alignment = haLeft;
}


Text::~Text() {
	if (texture) SDL_DestroyTexture(texture);
}

void Text::setText(std::string sText) {
	text = sText;
	RenderText();

}
void Text::RenderText() {
	if (!renderer) return;
	if (texture) SDL_DestroyTexture(texture);
	SDL_Surface* lSurface;

	int fontStyle = TTF_STYLE_NORMAL;
	if (underlined) fontStyle |= TTF_STYLE_UNDERLINE;
	if (bold) fontStyle |= TTF_STYLE_BOLD;

	TTF_SetFontStyle(font, fontStyle);

	if (wrapLength > 0) lSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, wrapLength);
	else lSurface = TTF_RenderText_Blended(font, text.c_str(), color);

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	if (lSurface != NULL) {
		rect.w = lSurface->w;
		rect.h = lSurface->h;
		texture = SDL_CreateTextureFromSurface(renderer, lSurface);

		SDL_FreeSurface(lSurface);
	}
}


void Text::render() {
	SDL_Rect textRect = getTextRect();;

	SDL_RenderCopy(renderer, texture, NULL, &textRect);
}


SDL_Rect Text::getTextRect() {
	SDL_Rect textRect;
	switch (alignment) {
	case haLeft:
		textRect = { rect.x, rect.y, rect.w, rect.h };
		break;
	case haCenter:
		textRect = { rect.x - (rect.w / 2), rect.y, rect.w, rect.h };
		break;
	case haRight:
		textRect = { rect.x - rect.w, rect.y, rect.w, rect.h };
		break;
	default:
		textRect = { rect.x, rect.y, rect.w, rect.h };
		break;
	}
	return textRect;
}

int Text::getWidth() {
	return rect.w;
}

int Text::getHeight() {
	return rect.h;
}

void Text::setPosition(int px, int py) {
	rect.x = px;
	rect.y = py;
}

void Text::setX(int x) {
	rect.x = x;
}

void Text::setY(int y) {
	rect.y = y;
}