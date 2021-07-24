#include "stdafx.h"
#include "Client.h"

#include "GameMap.h"
#include "UserManager.h"
#include "Player.h"
#include "MainWindow.h"

#include "pMapInfo.h"
#include "pPetShop.h"
#include "pAction.h"

pMapInfo::pMapInfo(char* buf, char* encBuf) {
	description = "Map Info (Server)";
	type = ptMapInfo;
	size = 52;
	initBuffer(size);
	memcpy(buffer, buf, size);
	memcpy(encryptedBuffer, encBuf, size);

	getWord(0, &verifNum);
	getInt(4, &userId);
	getInt(8, &mapId);
	getInt(12, &mapDoc);

	getWord(16, &mapX);
	getWord(18, &mapY);
	getWord(20, &weather);
	getWord(22, &bgMusic);
	getWord(24, &bgMusic_show);
	getWord(26, &fightMusic);

	getInt(28, &mapType);

	getString(32, mapName, 16);
}


pMapInfo::~pMapInfo() {
	//
}


void pMapInfo::process() {
	std::string msg = "Loading Map " + std::string(mapName);
	gClient.addToDebugLog(msg);
	bool firstMap = true;
	bool jumpMode = false;
	SDL_Rect mapUiRect;
	if (map) {
		mapUiRect = map->uiRect;
		jumpMode = map->getJumpMode();
		GameMap* pTemp = map;
		map = nullptr;
		delete pTemp;
		firstMap = false;
	}

	try {
		map = new GameMap(this);
	}
	catch (...) {
		gClient.addToDebugLog("Map '" + std::string(mapName) + "' failed to load. Terminating application.");
		showErrorMessage("Map '" + std::string(mapName) + "' failed to load. To application will now shut down.");
		applicationClose();
		return;
	}
	if (!firstMap) map->setMapUiRect(mapUiRect);
	if (jumpMode) map->toggleJumpMode();
	player->SetCoord(SDL_Point{ mapX, mapY });

	if (firstMap) {
		gClient.addToDebugLog("First Map Loaded");
		gClient.mainReady = true;

		//On initial login, a petshop packet is always sent
		pPetShop* petShopPacket = new pPetShop(10);
		gClient.addPacket(petShopPacket);

		pAction* movePacket = new pAction(gClient.accountId, player->GetID(), 0, 0, 0, 19);
		gClient.addPacket(movePacket);
	}
	userManager.clear();
}


void pMapInfo::debugPrint() {
	hideEncryptedBuffer = true;
	Packet::debugPrint();

	std::cout << mapName << " ID:" << mapId << " MapDoc:" << mapDoc << " Map Type:" << mapType << std::endl;
	std::cout << "Position: " << mapX << "," << mapY << std::endl;
	std::cout << "Weather:" << weather << " BG Music:" << bgMusic << " BG(Show) Music:" << bgMusic_show << " Battle Music:" << fightMusic << std::endl;

	std::cout << std::endl;
}