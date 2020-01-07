#include "stdafx.h"
#include "Texture.h"
#include "Define.h"
#include "Client.h"

Texture::Texture(SDL_Renderer* aRenderer) {
	renderer = aRenderer;
	setAnchor(aTopLeft);
}


Texture::Texture(SDL_Renderer* aRenderer, SDL_Texture* txtr, int w, int h) {
	renderer = aRenderer;
	texture = txtr;
	width = w;
	height = h;
	setAnchor(aTopLeft);
}


Texture::Texture(SDL_Renderer* aRenderer, std::string filePath, bool load) {
	renderer = aRenderer;
	file = filePath;
	setAnchor(aTopLeft);
	if (load) Load();
}


Texture::Texture(SDL_Renderer* aRenderer, std::string filePath, SDL_Color aColorKey, bool load) {
	renderer = aRenderer;
	file = filePath;
	setAnchor(aTopLeft);
	useColorKey = true;
	colorKey = aColorKey;
	if (load) Load();
}


Texture::~Texture() {
	if (texture) SDL_DestroyTexture(texture);
	if (surface && surface != NULL) SDL_FreeSurface(surface);
	if (rle) delete rle;
}


void Texture::Render() {
	Render({ 0, 0 });
}

void Texture::Render(SDL_Point p) {
	if (skip) return;
	if (!texture && !loaded && !loadFailed) Load();

	SDL_Rect rRect = rect;
	rRect.x += p.x + position.x;
	rRect.y += p.y + position.y;
	SDL_RenderCopy(renderer, texture, NULL, &rRect);
}


void Texture::Load() {
	if (skip || texture) return;

	if (!file.length()) {
		loadFailed = true;
		skip = true;
		return;
	}

	loadTexture(file);
}


void Texture::Reload() {
	if (surface) SDL_FreeSurface(surface);
	surface = NULL;
	if (texture) SDL_DestroyTexture(texture);
	texture = NULL;
	loaded = false;
	Load();
}


void Texture::loadResource(std::string filePath) {
	loadTexture(filePath);
}


void Texture::loadResource(std::string filePath, int anchor) {
	file = filePath;
	struct stat buffer;
	if (stat(filePath.c_str(), &buffer) != 0) {
		//std::cout << "File not Found: " << filePath << "\n";
		return;
	}
	int start, load1, load2=0;

	start = SDL_GetTicks();
	surface = IMG_Load(filePath.c_str());
	load1 = SDL_GetTicks() - start;
	if (surface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", filePath, SDL_GetError());
	}
	else {
		format = surface->format;
		if (useColorKey) SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(format, colorKey.r, colorKey.g, colorKey.b));
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		load2 = SDL_GetTicks() - start;
		width = surface->w;
		height = surface->h;

		int x;
		if (anchor == Anchor::aTopRight || anchor == Anchor::aBottomRight) x = -width;
		else x = 0;

		int y;
		if (anchor == Anchor::aBottomLeft || anchor == Anchor::aBottomRight) y = -height;
		else y = 0;

		rect = { x, y, width, height };

		//SDL_FreeSurface(uiSurface);
	}

	float time1 = load1 / 1000.0;
	float total = load2 / 1000.0;
	float time2 = total - time1;
	//std::cout << "IMG_Load: " << time1 << ", Create Texture: " << time2 << " - Total: " << total << "(" << filePath  << ")\n";
}

SDL_Surface* Texture::getSurface() {
	return surface;
}


SDL_Texture* Texture::getTexture() {
	if (!texture) return nullptr;
	return texture;
}


void Texture::loadSurface(std::string filePath) {
	file = filePath;
	loadSurface();
}

void Texture::loadSurface() {
	if (skip || loaded) return;
	if (!file.length()) {
		loadFailed = true;
		skip = true;
		return;
	}

	int extPos = file.find_last_of(".");
	std::string ext = file.substr(extPos + 1, std::string::npos);

	//check for hsl ID
	int hslIdPos = ext.find("_");
	if (hslIdPos != std::string::npos) {
		int origLength = ext.length();
		ext = ext.substr(0, hslIdPos);
		file = file.substr(0, file.length() - (origLength - hslIdPos));
	}

	int fileSize;
	std::shared_ptr<DataBuffer> fileBuffer;
	gClient.getFileFromWDF(file, fileBuffer, fileSize);

	transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	if (ext == "RLE") {
		if (!rle) {
			if (fileBuffer) rle = new RLE(file, fileBuffer.get()->buffer);
			else rle = new RLE(file);
			if (rle->load()) {
				if (hsbSetId > 0) {
					rle->addHsbSets(hsbSets, hsbSetCount);
					rle->reloadColorMap(true);
				}
				else {
					rle->reloadColorMap(false);
				}
				rle->createBitmap();
			}
		}
		else {
			if (reloadColorMap) {
				rle->reloadColorMap((hsbSetId > 0) ? true : false);
				rle->createBitmap();
			}
		}

		if (rle->loaded) {
			SDL_RWops* rwop = SDL_RWFromMem(rle->bitmap, rle->bitmapSize);
			surface = SDL_LoadBMP_RW(rwop, 0);
			SDL_RWclose(rwop);
		}
	}
	else if (ext == "BMP") {
		if (fileBuffer) {
			SDL_RWops* rwop = SDL_RWFromMem(fileBuffer.get()->buffer, fileSize);
			surface = SDL_LoadBMP_RW(rwop, 0);
			SDL_RWclose(rwop);
		}
	}
	else if (ext == "JPG" || ext == "PNG") {
		if (fileBuffer) {
			SDL_RWops* rwop = SDL_RWFromMem(fileBuffer.get()->buffer, fileSize);
			surface = IMG_Load_RW(rwop,1);
			SDL_RWclose(rwop);
		}
	}
	else if (ext == "TGA") {
		if (fileBuffer.get()) {
			TGA* tga = new TGA(file, fileBuffer.get()->buffer, fileSize);
			SDL_RWops* rwop = SDL_RWFromMem(tga->getBitmap(), tga->getBitmapSize());
			surface = SDL_LoadBMP_RW(rwop, 0);
			SDL_RWclose(rwop);
			delete tga;
		}
	}
	else {
		std::cout << "Unknown file extension for: " << file << std::endl;
	}

	if (surface == NULL) {
		//precheck for file in path?
		struct stat buffer;
		if (stat(file.c_str(), &buffer) != 0) {
			std::cout << "File not Found: " << file << "\n";
			skip = true;
			loadFailed = true;
			return;
		}

		surface = IMG_Load(file.c_str());
	}

	if (surface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", file, SDL_GetError());
		skip = true;
		loadFailed = true;
		return;
	}
	else loaded = true;

	format = surface->format;
	width = surface->w;
	height = surface->h;

	setAnchor(anchor); //also reloads rect from anchor
}


void Texture::loadTexture(std::string filePath) {
	file = filePath;
	if (!surface) loadSurface(filePath);

	if (surface == NULL) return;

	if (useColorKey) SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(format, colorKey.r, colorKey.g, colorKey.b));
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (useBlend) SDL_SetTextureBlendMode(texture, blendMode);
}


void Texture::setTextureOffset(int x, int y) {
	rect.x = x;
	rect.y = y;
}


void Texture::setBlendMode(SDL_BlendMode bm) {
	blendMode = bm;
	useBlend = true;
	if (texture) {
		SDL_SetTextureBlendMode(texture, blendMode);
	}
}


Uint32 Texture::getPixel(SDL_Point pixelPos) {
	Uint32 pixel;

	SDL_LockSurface(surface);
	Uint32 *pixels = (Uint32*)surface->pixels;
	int pos = (pixelPos.y * width) + pixelPos.x;
	pixel = pixels[pos];
	SDL_UnlockSurface(surface);

	return pixel;
}


Uint8 Texture::getPixelAlpha(Uint32 pixel) {
	Uint8 alpha;
	Uint32 temp = pixel & format->Amask;
	temp = temp >> format->Ashift;
	temp = temp << format->Aloss;
	alpha = (Uint8)temp;

	return alpha;
}


SDL_Point Texture::getPosition() {
	return position;
}

void Texture::setPosition(SDL_Point p) {
	position = p;
}


void Texture::setAnchor(int aAnchor) {
	anchor = aAnchor;

	int x;
	if (anchor == Anchor::aTopRight || anchor == Anchor::aBottomRight) x = -width;
	else x = 0;

	int y;
	if (anchor == Anchor::aBottomLeft || anchor == Anchor::aBottomRight) y = -height;
	else y = 0;

	rect = { x, y, width, height };
}


void Texture::setHsbShifts(HSBSet* sets, int count, int id) {
	hsbSetCount = count;
	hsbSetId = id;

	for (int i = 0; i < count; i++) {
		hsbSets[i] = sets[i];
	}
}


void Texture::reloadWithHsbShifts(std::map<int, ColorShift> shifts) {
	for (auto shift : shifts) {
		ColorShift doShift = shift.second;
		rle->addHSBShift(doShift.hue, doShift.newHue, doShift.range, doShift.sat, doShift.bright);
	}

	SDL_FreeSurface(surface);
	surface = NULL;
	SDL_DestroyTexture(texture);
	texture = NULL;

	loadTexture(file);
}