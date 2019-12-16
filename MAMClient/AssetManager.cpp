#include "stdafx.h"
#include "AssetManager.h"

#include "Texture.h"

AssetManager::AssetManager() {
	aMutex = SDL_CreateMutex();
	qMutex = SDL_CreateMutex();
}

AssetManager::~AssetManager() {
	//free all assets
}


void AssetManager::addAsset(void* context, Asset asset) {
	SDL_LockMutex(aMutex); {
		Texture* t = asset.get();
		assetsByContext[context][t->file] = asset;
	} SDL_UnlockMutex(aMutex);
}

void AssetManager::queueAsset(Asset asset) {
	Texture* t = asset.get();
	if (!t) return;

	SDL_LockMutex(qMutex); {
		queue.push_back(asset);
	} SDL_UnlockMutex(qMutex);
}

void AssetManager::addAndQueueAsset(void *context, Asset asset) {
	addAsset(context, asset);
	queueAsset(asset);
}


void AssetManager::releaseAssets(void *context) {
	if (!context) return;

	SDL_LockMutex(aMutex);
	ContextItr itr;
	itr = assetsByContext.find(context);
	if (itr == assetsByContext.end()) return;

	AssetMap* assetMap = &itr->second;
	assetMap->clear();
	assetsByContext.erase(context);
	SDL_UnlockMutex(aMutex);
}

void AssetManager::loadAssets() {
	Asset asset = getNextAssetFromQueue();

	Texture *texture = asset.get();
	if (texture) texture->loadSurface();
	return;
}

Asset AssetManager::getNextAssetFromQueue() {
	Asset asset;
	SDL_LockMutex(qMutex); {
		if (queue.size()) {
			asset = queue.front();
			queue.erase(queue.begin());
		}
	} SDL_UnlockMutex(qMutex);

	return asset;
}