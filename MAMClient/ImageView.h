#ifndef __IMAGEVIEW_H
#define __IMAGEVIEW_H

#include "Widget.h"

class ImageView : public Widget {
public:
	ImageView(int toX, int toY, int w, int h);
	~ImageView();

	void ImageView::render();
	void ImageView::setTexture(Texture* texture);
	void ImageView::clearTexture();

private:
	SDL_Texture* renderTexture;
	SDL_Texture *ivTexture;
	Texture* imageTexture = nullptr;
	SDL_Rect imageRect;
};

#endif