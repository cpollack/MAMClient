#include "stdafx.h"
#include "Sprite.h"

Sprite::Sprite(SDL_Renderer* aRenderer, int spriteType) {
	renderer = aRenderer;
	type = spriteType;
	speed = 1000;
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType) {
	std::vector<std::string> files;
	files.push_back(file);
	init(aRenderer, files, 1, 0, 0, spriteType);
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::string file, int spriteType, ColorShifts shifts) {
	std::vector<std::string> files;
	files.push_back(file);
	setHsbShifts(shifts);
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

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int spriteType, ColorShifts shifts) {
	setHsbShifts(shifts);
	init(aRenderer, file, file.size(), 0, 0, spriteType);
}

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<std::string> file, int toX, int toY) {
	init(aRenderer, file, file.size(), toX, toY, stCharacter);
}


Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType) {
	renderer = aRenderer;
	type = spriteType;
	if (type == stEffect) repeatMode = 1;
	speed = 1000;
	if (spriteType == stNpc) speed = 1000;

	frames = textures.size();
	if (frames > 0) spr_rect = new SDL_Rect[frames];
	subimages = textures;
}

Sprite::Sprite(SDL_Renderer* aRenderer, std::vector<Asset> textures, int spriteType, ColorShifts shifts) {
	renderer = aRenderer;
	type = spriteType;
	if (type == stEffect) repeatMode = 1;
	speed = 1000;
	if (spriteType == stNpc) speed = 1000;

	frames = textures.size();
	if (frames > 0) spr_rect = new SDL_Rect[frames];
	subimages = textures;

	setHsbShifts(shifts);
	/*if (colorShifts.size() > 0) {
		for (auto subimage : subimages) {
			subimage->setHsbShifts(shifts);
		}
	}*/
}


Sprite::~Sprite() {
	//if (subimages.size()) {
	//	for (int i = 0; i < frames; i++) {
			//delete subimages.at(i);
			//handle texture deletion through another method?

			//if (hslShiftId > 0) gClient.deleteTexture(subimages[i]->file, hslShiftId);
			//else gClient.deleteTexture(subimages[i]->file);
		//}
		//subimages.clear();
	//}
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

	speed = 1000;
	if (spriteType == stNpc) speed = 1000;

	frames = frameCount;
	if (frames > 0) spr_rect = new SDL_Rect[frames];
	for (int i = 0; i < frames; i++) {
		Asset next(new Texture(renderer, file[i], false));
		if (colorShifts.size() > 0) next->setHsbShifts(colorShifts);
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
	
	DWORD currentTime = SDL_GetTicks();
	DWORD elapsed = currentTime - startTime;
	
	if (!started) {
		if (LoopTimer > 0) {
			DWORD compareTime = currentTime - LastLoopTime;
			if (compareTime < LoopTimer) return;
			else {
				LastLoopTime = currentTime;
				start();
				elapsed = currentTime - startTime;
			}
		}
		else return;
	}

	//Delay start of sprite by MS
	if (started && StartDelay > 0) {
		if (elapsed < StartDelay) return;
	}

	//Refactor - Speed respresents length of time to do one full cycle of the sprite
	//More frames means more detail over the animation, but also more loading

	//repeatCount = elapsed / speed;
	//int nextFrame =  floor((elapsed - (repeatCount * speed)) / (speed * 1.0 / frames));

	//refactor 2, speed/frameInterval is flat 50ms assumed. 

	int nextFrame = currentFrame;
	if (bStopOnNextLoop && currentFrame == 0) return;

	if (currentTime - lastFrame > frameInterval) {
		lastFrame = currentTime;
		currentFrame++;
		if (currentFrame >= frames) {
			currentFrame = 0;
			repeatCount++;
		}
		nextFrame = currentFrame;
	}

	//int framesElapsed = elapsed / frameInterval;
	//repeatCount = framesElapsed / frames;
	//int nextFrame = framesElapsed % frames;


	if (repeatMode > 0) {
		if (repeatCount >= repeatMode) {
			nextFrame = frames - 1;
			isFinished = true;
			stop();
			if (type == stEffect) return; //Do not render completed effects
		}
	}

	Asset frameTexture = subimages.at(nextFrame);
	if (frameTexture->texture == nullptr) frameTexture->Load();

	SDL_Rect renderRect = getRenderRect(nextFrame);
	bool inView = true;
	if (camera) inView = SDL_IntersectRect(camera, &renderRect, NULL);
	if (!inView) return;

	if (frameTexture->reloadColorMap) frameTexture->reloadWithHsbShifts(colorShifts);

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
	startTime = SDL_GetTicks();
	lastFrame = startTime;
	started = true;
	isFinished = false;
	bStopOnNextLoop = false;
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

void Sprite::RandomizeTimerDelay() {
	LastLoopTime = SDL_GetTicks() - (rand() % LoopTimer);
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
	if (subimages.size() == 0 || subimages.size() < currentFrame) return{ 0,0 };

	SDL_Rect renderRect = getRenderRect(currentFrame);

	Asset frameTexture = subimages.at(currentFrame);
	if (!frameTexture->getSurface() && !frameTexture->skip) frameTexture->loadSurface();
	SDL_Rect sprRect = frameTexture->getRect();
	renderRect.x += sprRect.x;
	renderRect.y += sprRect.y;

	return renderRect;
}

SDL_Rect Sprite::getRenderRect(int frame) {
	if (subimages.size() == 0 || subimages.size() < frame) return{ 0,0 };
	Asset frameTexture = subimages.at(frame);
	if (!frameTexture->getSurface() && !frameTexture->skip) frameTexture->loadSurface();

	SDL_Rect sprRect = frameTexture->getRect();
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
		//renderRect = { x + sprRect.x, y + sprRect.y, sprRect.w, sprRect.h };
		break;
	case stCloud:
		renderRect = { x - (int)(sprRect.w / 2), y - (int)(sprRect.h / 2), sprRect.w, sprRect.h };
		break;
	default:
		renderRect = { x, y, sprRect.w, sprRect.h };
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


void Sprite::setHsbShifts(ColorShifts shifts) {
	colorShifts = shifts;

	for (auto asset : subimages) {
		asset->reloadColorMap = true;
		asset->setHsbShifts(shifts);
	}
}