#include "stdafx.h"
#include "Sprite.h"

Sprite::Sprite(SDL_Renderer* aRenderer, int spriteType) {
	renderer = aRenderer;
	type = spriteType;
	speed = 12;
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType) {
	std::vector<std::string> files;
	files.push_back(file);
	init(aRenderer, files, 1, 0, 0, spriteType);
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType, HSBSet* hsbSets, int hsbCount) {
	std::vector<std::string> files;
	files.push_back(file);
	setHsbShifts(hsbSets, hsbCount);
	init(aRenderer, files, 1, 0, 0, spriteType);
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::string file, int toX, int toY) {
	std::vector<std::string> files;
	files.push_back(file);
	init(aRenderer, files, 1, toX, toY, stCharacter);
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::string* file, int frameCount, int toX, int toY) {
	std::vector<std::string> files;
	for (int i = 0; i < frames; i++) {
		files.push_back(file[i]);
	}
	init(aRenderer, files, frameCount, toX, toY, stCharacter);
}

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType) {
	init(aRenderer, file, file.size(), 0, 0, spriteType);
}

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType, HSBSet* hsbSets, int hsbCount) {
	setHsbShifts(hsbSets, hsbCount);
	init(aRenderer, file, file.size(), 0, 0, spriteType);
}

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int toX, int toY) {
	init(aRenderer, file, file.size(), toX, toY, stCharacter);
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType) {
	renderer = aRenderer;
	type = spriteType;
	if (type == stEffect) repeatMode = 1;
	speed = 12;
	if (spriteType == stNpc) speed = 30;

	frames = textures.size();
	if (frames > 0) spr_rect = new SDL_Rect[frames];
	subimages = textures;
}

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType, HSBSet* hsbSets, int hsbCount) {
	renderer = aRenderer;
	type = spriteType;
	if (type == stEffect) repeatMode = 1;
	speed = 12;
	if (spriteType == stNpc) speed = 30;

	frames = textures.size();
	if (frames > 0) spr_rect = new SDL_Rect[frames];
	subimages = textures;

	setHsbShifts(hsbSets, hsbCount);
	if (hsbShiftId > 0) {
		for (auto subimage : subimages) {
			subimage->setHsbShifts(hsbSets, hsbSetCount, hsbShiftId);
		}
	}
}


Sprite::~Sprite() {
	if (subimages.size()) {
		for (int i = 0; i < frames; i++) {
			//delete subimages.at(i);
			//handle texture deletion through another method?

			//if (hslShiftId > 0) gClient.deleteTexture(subimages[i]->file, hslShiftId);
			//else gClient.deleteTexture(subimages[i]->file);
		}
		subimages.clear();
	}
	if (spr_rect) delete[] spr_rect;
}


void Sprite::deleteTextures() {
	//Subimages is a list of shared pointers. The associated texture will automatically be freed when appropriate
}


void Sprite::init(SDL_Renderer* aRenderer, std::vector<std::string> file, int frameCount, int toX, int toY, int spriteType) {
	renderer = aRenderer;
	fileList = file;
	x = toX;
	y = toY;
	type = spriteType;
	if (type == stEffect) repeatMode = 1;

	speed = 12;
	if (spriteType == stNpc) speed = 30;

	frames = frameCount;
	if (frames > 0) spr_rect = new SDL_Rect[frames];
	for (int i = 0; i < frames; i++) {
		Asset next(new Texture(renderer, file[i], false));
		if (hsbShiftId > 0) next->setHsbShifts(hsbSets, hsbSetCount, hsbShiftId);
		subimages.push_back(Asset(next));
	}
}


void Sprite::addFrame(Asset newFrame) {
	frames++;
	subimages.push_back(newFrame);
}


void Sprite::LoadFirst() {
	if (subimages.size() == 0) return;
	subimages[0]->Load();
}


void Sprite::render() {
	render(0, 0);
}


void Sprite::render(int offsetX, int offsetY) {
	if (!visible) return;
	if (subimages.size() == 0) return;

	int nextFrame = ((frameCounter + 1) / speed) % frames;
	Asset frameTexture = subimages.at(nextFrame);
	if (frameTexture->texture == nullptr) frameTexture->Load();		

	SDL_Rect renderRect = getRenderRect(nextFrame);
	bool inView = true;
	if (camera) inView = SDL_IntersectRect(camera, &renderRect, NULL);
	if (!inView) return;

	if (started) frameCounter++;

	if (repeatMode > 0) {
		if (type == stEffect && repeatCount >= repeatMode) {
			isFinished = true;
			return;
		}

		if (repeatCount < repeatMode) {
			currentFrame = (frameCounter / speed) % frames;
			repeatCount = (frameCounter / speed) - frames + 2;
		}
	}
	else {
		currentFrame = (frameCounter / speed) % frames;
		repeatCount = (frameCounter / speed) - frames + 2;
	}

	if (colorShifts.size() > 0) frameTexture->reloadWithHsbShifts(colorShifts);

	int alphaBlendResult = SDL_SetTextureAlphaMod(frameTexture->texture, alpha);
	
	//renderRect.x += frameTexture->position.x;
	//renderRect.y += frameTexture->position.y;
	frameTexture->Render(SDL_Point{ renderRect.x + offsetX, renderRect.y + offsetY});
	//SDL_RenderCopy(renderer, frameTexture->texture, NULL, &renderRect);
}


void Sprite::start() {
	currentFrame = 0;
	frameCounter = 0;
	repeatCount = 0;
	started = true;
	isFinished = false;
}

void Sprite::resume() {
	started = true;
}

void Sprite::stop() {
	started = false;
}

bool Sprite::finished() {
	return isFinished;
}

void Sprite::setCamera(SDL_Rect c) {
	if (!camera) camera = new SDL_Rect;
	*camera = c;
}

void Sprite::setLocation(SDL_Point p) {
	x = p.x;
	y = p.y;
}

void Sprite::setLocation(int toX, int toY) {
	x = toX;
	y = toY;
}

void Sprite::setAlpha(BYTE newAlpha) {
	alpha = newAlpha;
}

SDL_Rect Sprite::getRenderRect() {
	return getRenderRect(currentFrame);
}


SDL_Rect Sprite::getRenderRect(int frame) {
	if (subimages.size() == 0 || subimages.size() < frame) return{ 0,0 };
	Asset frameTexture = subimages.at(frame);
	if (!frameTexture->getSurface() && !frameTexture->skip) frameTexture->loadSurface();

	SDL_Rect sprRect = frameTexture->rect;
	SDL_Rect renderRect;
	switch (type) {
	case stCharacter:
	case stNpc:
	case stMonster:
	case stEffect:
		renderRect = { (x - (int)(sprRect.w / 2)), (y - (int)(sprRect.h * 0.75)), sprRect.w, sprRect.h };
		break;
	case stObject:
	case stStatic:
		renderRect = { x, y, sprRect.w, sprRect.h };
		break;
	}

	return renderRect;
}


Asset Sprite::getCurrentTexture() {
	return subimages.at(currentFrame);
}


Sprite* Sprite::copy() {
	Sprite* newSprite;
	newSprite = new Sprite(renderer, type);
	newSprite->subimages = subimages;
	newSprite->frames = frames;
	newSprite->fileList = fileList;
	newSprite->speed = speed;
	return newSprite;
}


void Sprite::setHsbShifts(HSBSet* sets, int count) {
	hsbSetCount = count;

	for (int i = 0; i < count; i++) {
		hsbSets[i] = sets[i];
	}

	hsbShiftId = 0;
	for (int i = 0; i < count * 5; i++) {
		hsbShiftId += *((BYTE*)hsbSets + i);
	}
}