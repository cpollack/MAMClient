#include "stdafx.h"
#include "MapFile.h"
#include "GameLibrary.h"

MapFile::MapFile(int mapId) {
	id = mapId;

	std::string iniSection = "Map" + std::to_string(id);
	INI mapIni("ini\\GameMap.ini", iniSection);
	std::string mapPath = mapIni.getEntry("File");

	if (mapPath.at(0) == '.') {
		mapPath = mapPath.substr(2);
	}
	
	if (!fileExist(mapPath)) {
		std::string msg = "Map File " + mapPath + " is missing. The application will now shutdown.";
		showErrorMessage(msg);
		applicationClose();
	}

	std::ifstream mapFile(mapPath, std::ios::binary);
	if (!mapFile.is_open()) return;

	//Read Header section: Cipher, Verification Key, W/H and Image Path
	int cipher;
	mapFile.read((char*)&cipher, 4);

	unsigned int filetype;
	mapFile.read((char*)&filetype, 4);
	if (filetype != 2022144135) return;

	mapFile.read((char*)&width, 4); width ^= cipher;
	mapFile.read((char*)&height, 4); height ^= cipher;
	
	char aPath[256];
	mapFile.read(aPath, 256);
	for (int a = 0; a < strlen(aPath); a++) aPath[a] ^= cipher;
	imagePath = aPath;

	//Coordinates
	coordinates = new char[width * height];
	mapFile.read(coordinates, (width * height));
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int pos = (y*width) + x;
			coordinates[pos] ^= cipher + x;
		}
	}

	//Masks
	int maskCount;
	mapFile.read((char*)&maskCount, 4);
	maskCount ^= cipher;
	if (maskCount > 0) {
		for (int m = 0; m < maskCount; m++) {
			MapMask mask;
			char maskName[64];
			mapFile.read(maskName, 64);

			for (int i = 0; i < strlen(maskName); i++) maskName[i] ^= cipher;
			mask.file = maskName;

			mapFile.read((char*)&mask.xOffset, 24);
			mask.xOffset ^= cipher;
			mask.yOffset ^= cipher;
			mask.blockCoordDistanceX ^= cipher;
			mask.blockCoordDistanceY ^= cipher;
			mask.u3 ^= cipher;
			mask.u4 ^= cipher;
			masks.push_back(mask);
		}
	}

	//Objects
	int objCount;
	mapFile.read((char*)&objCount, 4);
	objCount ^= cipher;
	if (objCount > 0) {
		for (int c = 0; c < objCount; c++) {
			MapObject obj;
			mapFile.read((char*)&obj, 12);

			obj.id ^= cipher;
			obj.x ^= cipher;
			obj.y ^= cipher;
			objects.push_back(obj);
		}
	}

	//Portal
	int portalCount;
	mapFile.read((char*)&portalCount, 4);
	portalCount ^= cipher;
	if (portalCount <= 32) {
		if (portalCount > 0) {
			MapPortal portal;
			for (int d = 0; d < portalCount; d++) {
				mapFile.read((char*)&portal, 12);

				portal.id ^= cipher;
				portal.x ^= cipher;
				portal.y ^= cipher;
				portals.push_back(portal);
			}
		}
	}
}


MapFile::~MapFile() {
	masks.clear();
	objects.clear();
	portals.clear();
	delete[] coordinates;
}