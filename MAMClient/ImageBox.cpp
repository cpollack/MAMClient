#include "stdafx.h"
#include "Widget.h"
#include "ImageBox.h"
#include "Sprite.h"

CImageBox::CImageBox(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;
}

CImageBox::CImageBox(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("SkinImage")) SkinImage = vWidget["SkinImage"].GetString();
	else SkinImage = "";

	if (vWidget.HasMember("Bordered")) Bordered = vWidget["Bordered"].GetBool();
	if (vWidget.HasMember("BlackBackground")) BlackBackground = vWidget["BlackBackground"].GetBool();
}

CImageBox::~CImageBox() {
	if (ImageBox) SDL_DestroyTexture(ImageBox);
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
	//
}

void CImageBox::SetImageFromSkin(std::string skinImage) {
	Texture* sImage = gui->getSkinTexture(renderer, skinImage, Anchor::TOP_LEFT);
	if (sImage) {
		SetImage(sImage);
		SkinImage = skinImage;
		delete sImage;
	}
}

void CImageBox::SetImage(Texture* image) {
	SkinImage = "";

	if (ImageBox) SDL_DestroyTexture(ImageBox);
	ImageBox = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_Color bgColor = gui->backColor;

	SDL_SetRenderTarget(renderer, ImageBox);

	//Draw the background
	if (BlackBackground) SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	else SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);

	SDL_RenderClear(renderer);

	if (Bordered) {
		hlineRGBA(renderer, 0, Width - 1, 0, 0xA0, 0xA0, 0xA0, 0xFF);
		vlineRGBA(renderer, 0, 0, Height - 1, 0xA0, 0xA0, 0xA0, 0xFF);

		hlineRGBA(renderer, 0, Width - 1, Height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, 0xFF, 0xFF, 0xFF, 0xFF);

		if (image) {
			int srcWidth = Width - 2;
			if (srcWidth < Width - 2) srcWidth = image->width;
			int srcHeight = Height - 2;
			if (srcHeight < Height - 2) srcHeight = image->height;
			int srcX = 0;
			if (srcWidth < image->width) srcX = (image->width / 2) - (srcWidth / 2);
			int srcY = 0;
			if (srcHeight < image->height) srcY = (image->height / 2) - (srcHeight / 2);
			SDL_Rect srcRect{ srcX, srcY, srcWidth, srcHeight };
			SDL_Rect renderRect{ 1, 1,
				image->width > Width - 2 ? Width - 2 : image->width,
				image->height > Height - 2 ? Height - 2 : image->height };
			SDL_RenderCopy(renderer, image->texture, &srcRect, &renderRect);
		}
	}
	else {
		if (image) {
			int srcWidth = Width;
			if (srcWidth < Width) srcWidth = image->width;
			int srcHeight = Height;
			if (srcHeight < Height) srcHeight = image->height;
			int srcX = 0;
			if (srcWidth < image->width) srcX = (image->width / 2) - (srcWidth / 2);
			int srcY = 0;
			if (srcHeight < image->height) srcY = (image->height / 2) - (srcHeight / 2);
			SDL_Rect srcRect{ srcX, srcY, srcWidth, srcHeight };
			SDL_Rect renderRect{ 1, 1,
				image->width > Width ? Width : image->width,
				image->height > Height ? Height : image->height };
			SDL_RenderCopy(renderer, image->texture, &srcRect, &renderRect);
		}
	}
	SDL_SetRenderTarget(renderer, NULL);
}

void CImageBox::BindSprite(Sprite* sprite) {
	this->sprite = sprite;
}
