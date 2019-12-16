#include "stdafx.h"
#include "ImageView.h"


ImageView::ImageView(int toX, int toY, int w, int h) : Widget(toX, toY) {
	width = w;
	height = h;

	ivTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_SetRenderTarget(renderer, ivTexture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//Border
	hlineRGBA(renderer, 0, width - 1, 0, 0xA0, 0xA0, 0xA0, 0xFF);
	vlineRGBA(renderer, 0, 0, height - 1, 0xA0, 0xA0, 0xA0, 0xFF);

	hlineRGBA(renderer, height - 1, 0, width - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	vlineRGBA(renderer, width - 1, 0, height -1, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderTarget(renderer, NULL);
}


ImageView::~ImageView() {
	SDL_DestroyTexture(ivTexture);
	SDL_DestroyTexture(renderTexture);
}


void ImageView::render() {
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_Rect ivRect = { 0, 0, width, height };
	SDL_RenderCopy(renderer, ivTexture, NULL, &ivRect);

	if (imageTexture) {
		SDL_Rect txRect;
		SDL_RenderCopy(renderer, imageTexture->texture, NULL, &imageRect);
	}

	SDL_SetRenderTarget(renderer, NULL);

	SDL_Rect renderRect = { x, y, width, height };
	SDL_RenderCopy(renderer, renderTexture, NULL, &renderRect);
}


void ImageView::setTexture(Texture* texture) {
	imageTexture = texture;
	if (!texture) return;
	imageRect = { 0, 0, imageTexture->width, imageTexture->height };

	if (imageTexture->width < width) {
		imageRect.x += (width - imageTexture->width) / 2;
	}
	if (imageTexture->height < height) {
		imageRect.y += (height - imageTexture->height) / 2;
	}
}

void ImageView::clearTexture() {
	imageTexture = nullptr;
}