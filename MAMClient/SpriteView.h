#ifndef __SPRITEVIEW_H
#define __SPRITEVIEW_H

#include "Widget.h"
#include "Sprite.h"

class SpriteView : public Widget {
public:
	SpriteView(int toX, int toY, int w, int h);
	~SpriteView();

	void SpriteView::render();

	void SpriteView::setLook(int lk);

private:
	SDL_Texture *renderTexture;
	SDL_Point sprPoint;
	std::map<std::string, Sprite*> sprites;
	Sprite *sprite = nullptr;

	int look;
	std::string role, animString;
	int animation;
	int direction;

	void SpriteView::loadSprite();
};

#endif