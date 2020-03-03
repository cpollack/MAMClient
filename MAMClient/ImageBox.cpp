#include "stdafx.h"
#include "Widget.h"
#include "ImageBox.h"
#include "Define.h"
#include "Sprite.h"

#include "Window.h"

CImageBox::CImageBox(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;
	widgetRect.x = X;
	widgetRect.y = Y;
}

CImageBox::CImageBox(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("SkinImage")) SkinImage = vWidget["SkinImage"].GetString();
	else SkinImage = "";

	if (vWidget.HasMember("Bordered")) Bordered = vWidget["Bordered"].GetBool();
	if (vWidget.HasMember("BlackBackground")) BlackBackground = vWidget["BlackBackground"].GetBool();

	if (vWidget.HasMember("Anchor")) Anchor = vWidget["Anchor"].GetInt();
}

CImageBox::~CImageBox() {
	if (ImageBox) SDL_DestroyTexture(ImageBox);
	if (sprite) delete sprite;
}

void CImageBox::ReloadAssets() {
	if (ImageBox) {
		if (SkinImage.length() > 0) SetImageFromSkin(SkinImage);
		else SetImage(nullptr);
	}
}

void CImageBox::Render() {
	if (!Visible) return;
	if (!ImageBox) {
		if (SkinImage.length() > 0) SetImageFromSkin(SkinImage);
		else SetImage(nullptr);
	}

	if (sprite) {
		SDL_RenderCopy(renderer, ImageBox, NULL, &widgetRect);

		//Clip sprite to render only inside the ImageBox
		SDL_Rect clipRect = widgetRect;
		clipRect.x += 1; clipRect.y += 1;
		clipRect.w -= 2; clipRect.h -= 2;
		SDL_RenderSetClipRect(renderer, &clipRect);

		SDL_Point pos;
		pos.x = clipRect.x + (clipRect.w / 2);
		pos.y = clipRect.y + (int)((double)clipRect.h * 0.75);
		sprite->render(pos.x, pos.y);

		SDL_RenderSetClipRect(renderer, NULL);
		
	}
	else SDL_RenderCopy(renderer, ImageBox, NULL, &widgetRect);
}

void CImageBox::HandleEvent(SDL_Event& e) {
	if (!Visible) return;
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		if (doesPointIntersect(widgetRect, mx, my)) {
			OnClick(e);
		}
	}
}

void CImageBox::SetImageFromSkin(std::string skinImage) {
	Texture* sImage = gui->getSkinTexture(renderer, skinImage, Anchor::aTopLeft);
	if (sImage) {
		SetImage(sImage);
		SkinImage = skinImage;
		delete sImage;
	}
}

void CImageBox::SetImage(Texture* image) {
	SkinImage = "";

	SDL_Rect oldViewport;
	SDL_RenderGetViewport(renderer, &oldViewport);

	if (ImageBox) SDL_DestroyTexture(ImageBox);
	ImageBox = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);

	SDL_Color bgColor = gui->backColor;

	SDL_SetRenderTarget(renderer, ImageBox);

	//Draw the background
	if (BlackBackground) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	SDL_RenderClear(renderer);
	SDL_Rect viewPortRect = { 0, 0,widgetRect.w, widgetRect.h };
	SDL_RenderSetViewport(renderer, NULL);
	if (Bordered) {
		hlineRGBA(renderer, 0, Width - 1, 0, 0xA0, 0xA0, 0xA0, 0xFF);
		vlineRGBA(renderer, 0, 0, Height - 1, 0xA0, 0xA0, 0xA0, 0xFF);

		hlineRGBA(renderer, 0, Width - 1, Height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, 0xFF, 0xFF, 0xFF, 0xFF);

		viewPortRect.x = 1;
		viewPortRect.y = 1;
		viewPortRect.w -= 2;
		viewPortRect.h -= 2;
	}

	SDL_RenderSetViewport(renderer, &viewPortRect);

	SDL_Rect srcRect;
	SDL_Rect destRect;

	int maxWidth = viewPortRect.w;
	int maxHeight = viewPortRect.h;
	if (image) {
		//Source Rectangle
		switch (Anchor) {
		case aTopLeft:
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = image->width > maxWidth ? maxWidth : image->width;
			srcRect.h = image->height > maxHeight ? maxHeight : image->height;
			break;
		case aCenter:
			srcRect.w = image->width > maxWidth ? maxWidth : image->width;
			srcRect.h = image->height > maxHeight ? maxHeight : image->height;
			srcRect.x = (image->width / 2) - (srcRect.w / 2);
			srcRect.y = (image->height / 2) - (srcRect.h / 2);
			break;
		}

		//Destination Rectangle, for now we always center
		destRect.w = srcRect.w;
		destRect.h = srcRect.h;
		destRect.x = (viewPortRect.w / 2) - (destRect.w / 2);
		destRect.y = (viewPortRect.h / 2) - (destRect.h / 2);

		SDL_RenderCopy(renderer, image->texture, &srcRect, &destRect);
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetTextureBlendMode(ImageBox, SDL_BLENDMODE_BLEND);
	SDL_RenderSetViewport(renderer, &oldViewport);
}

void CImageBox::BindSprite(Sprite* sprite) {
	if (this->sprite) {
		delete this->sprite;
	}
	this->sprite = sprite;
}

void CImageBox::OnClick(SDL_Event& e) {
	auto iter = eventMap.find("Click");
	if (iter != eventMap.end()) iter->second(e);
}

void CImageBox::UseBlackBackground(bool use) {
	BlackBackground = use;
	if (SkinImage.length() > 0) SetImageFromSkin(SkinImage);
	else SetImage(nullptr);
}