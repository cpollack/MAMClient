#ifndef __MAPFILE_H
#define __MAPFILE_H

#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "INI.h"

typedef struct MapMask {
	std::string file;
	int xOffset;
	int yOffset;
	int blockCoordDistanceX;
	int blockCoordDistanceY;
	int u3;
	int u4;
}MapMask;

typedef struct MapObject {
	int id;
	int x;
	int y;
}MapObject;

typedef struct MapPortal {
	int x;
	int y;
	int id;
}MapPortal;

class MapFile {
public:
	std::string mapName;
	std::string imagePath;
	int width;
	int height;
	char *coordinates;

	std::vector<MapMask> masks;
	std::vector<MapObject> objects;
	std::vector<MapPortal> portals;

	MapFile(int docID);
	~MapFile();
private:
	int id;
};

#endif