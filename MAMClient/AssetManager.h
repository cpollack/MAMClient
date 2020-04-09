#pragma once

#include "Define.h"

class Texture;
class RLE;

using AssetMap = std::map<std::string, Asset>;
using AssetItr = AssetMap::iterator;
using ContextMap = std::map<void*, AssetMap>;
using ContextItr = std::map<void*, AssetMap>::iterator;
using RLEAsset = std::shared_ptr<RLE>;
using RLEMap = std::map<std::string, RLEAsset>;
using RLEMapItr = RLEMap::iterator;

class AssetManager {
public:
	AssetManager();
	~AssetManager();

	void addAsset(void *context, Asset asset);
	void queueAsset(Asset asset);
	void addAndQueueAsset(void *context, Asset asset);

	void releaseAssets(void *context);

	void loadAssets();

	void addRLE(std::string path, RLEAsset rle);
	RLEAsset getRLE(std::string path);
	void releaseRLE(std::string path);

private:
	Asset getNextAssetFromQueue();

private:
	SDL_mutex *aMutex, *qMutex, *rleMutex;

	ContextMap assetsByContext;
	std::list<Asset> queue;

	RLEMap rleAssets;
};

extern AssetManager assetManager;