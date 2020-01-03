#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "RLE.h"
#include "TGA.h"

typedef enum Anchor {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
}Anchor;

class Texture {
public:
	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;
	RLE* rle = nullptr;
	std::string file;
	bool loaded = false, loadFailed = false;
	bool skip = false;
	bool managedAsset = false;

	SDL_PixelFormat* format;
	SDL_Rect rect;
	SDL_Point position = { 0, 0 };
	int anchor;

	int width;
	int height;
	bool useBlend = false;
	SDL_BlendMode blendMode;

	HSBSet hsbSets[7] = { 0 };
	int hsbSetCount = 0;
	int hsbSetId = 0;
	bool reloadColorMap = false;

	Texture(SDL_Renderer* aRenderer);
	Texture(SDL_Renderer* aRenderer, SDL_Texture* txtr, int w, int h);
	Texture(SDL_Renderer* aRenderer, std::string filePath, bool load = true);
	Texture(SDL_Renderer* aRenderer, std::string filePath, SDL_Color aColorKey, bool load = true);
	~Texture();

	void Render();
	void Render(SDL_Point p);

	void Load();
	void Texture::loadResource(std::string filePath);
	void Texture::loadResource(std::string filePath, int anchor);
	void Texture::setTextureOffset(int x, int y);
	void Texture::setBlendMode(SDL_BlendMode bm);

	SDL_Surface* Texture::getSurface();
	SDL_Texture* Texture::getTexture();
	void Texture::loadSurface();
	void Texture::loadSurface(std::string filePath);
	void Texture::loadTexture(std::string filePath);

	Uint32 Texture::getPixel(SDL_Point pixelPos);
	Uint8 Texture::getPixelAlpha(Uint32 pixel);

	SDL_Point getPosition();
	void setPosition(SDL_Point p);
	void setAnchor(int aAnchor);

	void Texture::setHsbShifts(HSBSet* sets, int count, int id);
	void Texture::reloadWithHsbShifts(std::map<int, ColorShift> shifts);

private:
	SDL_Renderer* renderer;
	bool useColorKey = false;
	SDL_Color colorKey;
};

#endif
