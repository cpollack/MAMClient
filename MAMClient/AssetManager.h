#pragma once

class Texture;

using Asset = std::shared_ptr<Texture>;
using AssetMap = std::map<std::string, Asset>;
using AssetItr = std::map<std::string, Asset>::iterator;
using ContextMap = std::map<void*, AssetMap>;
using ContextItr = std::map<void*, AssetMap>::iterator;

class AssetManager {
public:
	AssetManager();
	~AssetManager();

	void addAsset(void *context, Asset asset);
	void queueAsset(Asset asset);
	void addAndQueueAsset(void *context, Asset asset);

	void releaseAssets(void *context);

	void loadAssets();

private:
	Asset getNextAssetFromQueue();

private:
	SDL_mutex *aMutex, *qMutex;

	ContextMap assetsByContext;
	std::list<Asset> queue;
};

extern AssetManager assetManager;