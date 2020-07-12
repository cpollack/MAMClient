#ifndef __RLE_H
#define __RLE_H

#include "FreeImagePlus.h"
#include "Define.h"

struct RLEheader {
	DWORD id;
	DWORD width;
	DWORD height;
	WORD bpp;
	WORD planes;
	DWORD imageSize;
	DWORD buffer;
};

struct ColorMap {
	union {
		struct {
			int b;
			int g;
			int r;
		};
		struct {
			int blue;
			int green;
			int red;
		};
	};
};

struct ImageData {
	unsigned char mapPos;
	unsigned char alpha;
};

struct HSB {
	union {
		struct {
			unsigned char h;
			unsigned char s;
			unsigned char b;
		};
		struct {
			unsigned char hue;
			unsigned char sat;
			unsigned char bright;
		};
	};
};

struct WPixel {
	union {
		WORD color;				// 16bit
		struct {
			WORD blue : 5;		// 5bit
			WORD green : 6;		// 6bit
			WORD red : 5;		// 5bit
		};
	};
	WPixel() {}
	WPixel(WORD c) { color = (WORD)((c >> 8 & 0xf800) | (c >> 5 & 0x7e0) | (c >> 3 & 0x1f)); }
	WPixel(int r, int g, int b) : red(r >> 3), green(g >> 2), blue(b >> 3) {}
	operator DWORD() const { return (color << 5 & 0x7fc00) | (color << 8 & 0xf80000) | ((color << 3 | (color & 7)) & 0x3ff); }
};

/*struct HSBSet {
	BYTE hue;
	BYTE newHue;
	BYTE range;
	BYTE saturation;
	BYTE light;
};*/

class RLE {
public:
	//Bitmap *bitmap;
	unsigned char *buffer = nullptr;
	int fileSize;
	int colorMapSize;
	int bufferZoneSize;

	BYTE* bitmap = nullptr;
	int bitmapSize;

	bool newRle = false;
	bool loaded = false;
	int lastMapKey = -1;

	RLE(std::string file);
	RLE::RLE(std::string file, unsigned char *wdfBuffer);
	~RLE();

	bool RLE::load();
	void RLE::writeToPNG();
	void RLE::createBitmap();

	void RLE::addHsbSets(ColorShifts shifts);
	void RLE::addHSBShift(int hue, int newHue, int range, int saturation, int light);
	void RLE::addHueShift(BYTE newHue);
	void setColorShifts(ColorShifts shifts);
	void reloadColorMap(bool useHslShifts);
	int getCurrentMapKey();

private:
	RLEheader *header = nullptr;
	BYTE *bColorMap = nullptr;
	ColorMap *colorMap = nullptr;
	DWORD *bufferZone = nullptr;
	ImageData *image = nullptr;
	BYTE hueShift;

	const int bytesPerpixel = 2;
	bool flipped;

	std::string fileName, fileExt, filePath;
	std::string fullFilePath;
	ColorShifts colorShifts;

	WPixel RLE::HSBtoRGB(HSB hsb);
	void RLE::RGBtoPixel(WPixel* pixel, int r, int g, int b);

	void RLE::Rle3HSLtoRGB(WPixel* pixel, INT64* entry);
	void RLE::Rle3ConvertTo16BitRGB(WPixel* pixel, int r, int g, int b);

	bool RLE::shiftHSB(HSB* hsb, ColorShift hslShift);
};

#endif
