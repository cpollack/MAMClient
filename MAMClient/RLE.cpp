#include "stdafx.h"
#include "RLE.h"

RLE::RLE(std::string file) {
	fullFilePath = file;

	int fileNamePos = file.find_last_of('\\');
	int fileExtPos = file.find_last_of('.');

	filePath = file.substr(0, fileNamePos + 1);
	fileName = file.substr(fileNamePos + 1, fileExtPos - fileNamePos - 1);
	fileExt = file.substr(fileExtPos, std::string::npos);

	flipped = false;
}


RLE::RLE(std::string file, unsigned char *wdfBuffer) {
	fullFilePath = file;

	int fileNamePos = file.find_last_of('\\');
	int fileExtPos = file.find_last_of('.');

	filePath = file.substr(0, fileNamePos + 1);
	fileName = file.substr(fileNamePos + 1, fileExtPos - fileNamePos - 1);
	fileExt = file.substr(fileExtPos, std::string::npos);

	flipped = false;
	buffer = wdfBuffer;
}


RLE::~RLE() {
	delete header;
	delete[] bColorMap;
	delete[] colorMap;
	delete[] bufferZone;
	delete[] image;
	if (bitmap) delete[] bitmap;
	//buffer is a smart pointer which is deallocated automatically
}


bool RLE::load() {
	//std::cout << "Start RLE Load: " << fullFilePath << std::endl;
	int start = SDL_GetTicks();
	header = new RLEheader;
	colorMap = new ColorMap[256];

	//RLE parts
	unsigned char packetHead;
	unsigned char *rleChunk;

	if (!buffer) {
		std::ifstream ifs(fullFilePath, std::ios::binary | std::ios::ate);
		std::ifstream::pos_type pos = ifs.tellg();
		fileSize = pos;
		if (fileSize <= 0) {
			ifs.close();
			return false;
		}

		buffer = new unsigned char[fileSize];

		ifs.seekg(0, std::ios::beg);
		ifs.read((char*)buffer, fileSize);
		ifs.close();		
	}
	
	memcpy(header, buffer, sizeof(RLEheader));
	if (header->id != 17486) {
		std::cout << "Not a valid RLE: " << fullFilePath << std::endl;
		return false;
	}

	bufferZoneSize = 4 * header->height;
	colorMapSize = fileSize - sizeof(RLEheader) - bufferZoneSize - header->imageSize;
	if (fileSize == 0) {
		if (buffer[sizeof(RLEheader) + 3] == 0xCC) colorMapSize = 2048;
		else colorMapSize = 768;
		fileSize = sizeof(RLEheader) + bufferZoneSize + colorMapSize + header->imageSize;
	}

	int bufferOffset = sizeof(RLEheader);

	bColorMap = new BYTE[colorMapSize];
	memcpy(bColorMap, buffer + bufferOffset, colorMapSize);
	if ((colorMapSize / 256) == 3) newRle = false;
	else if ((colorMapSize / 256) == 8) newRle = true;
	else {
		std::cout << "Invalid RLE Color Map size: " << fullFilePath << std::endl;
		return false;
	}
	reloadColorMap(false);

	//Load the buffer zone. Contents of unknown significance
	bufferZone = new DWORD[header->height];
	bufferOffset += colorMapSize;
	memcpy((BYTE*)bufferZone, buffer + bufferOffset, bufferZoneSize);
	if (bufferZone[0] != 0) flipped = true;

	//Load the Run-Length Encoded Image data
	image = new ImageData[header->width * header->height];
	BYTE *data = new BYTE[header->imageSize];
	bufferOffset += bufferZoneSize;
	memcpy(data, buffer + bufferOffset, header->imageSize);

	//Parse Image Data - Packet Header followed by 2-byte pairs of pixels
	int bytesDone = 0;
	int pixel = 0;
	int maxPixels = header->width * header->height;
	while (bytesDone < header->imageSize && pixel < maxPixels) {
		packetHead = data[bytesDone++];

		//Run of the same pixel
		if ((packetHead & 0x80)) {
			packetHead &= 0x7F;
			packetHead++;

			ImageData runPixel;
			runPixel.mapPos = data[bytesDone++];
			runPixel.alpha = data[bytesDone++];

			for (unsigned char i = 0; i < packetHead; i++) {
				image[pixel++] = runPixel;
				if (pixel >= maxPixels) break;
			}
		}
		//Sequence of different pixels
		else {
			packetHead &= 0x7F;
			packetHead++;

			for (unsigned char i = 0; i < packetHead; i++) {
				image[pixel].mapPos = data[bytesDone++];
				image[pixel].alpha = data[bytesDone++];
				pixel++;
				if (pixel >= maxPixels) break;
			}
		}
	}

	float elapsed = (SDL_GetTicks() - start) / 10000.0;
	//std::cout << "RLE Load Time: " << elapsed << std::endl;

	loaded = true;
	return true;
}


WPixel RLE::HSBtoRGB(HSB hsb) {
	//Hue 0-192, Sat 0-64, Bright 0-64
	int B = 4 * hsb.b;
	if (!hsb.sat) return WPixel(B, B, B); 
	else {
		int P = B * (1.0f - (hsb.s / 64.0));

		float H = hsb.h / 192.0 * 6.0;
		float F = H - floorf(H);
		int Q = B * (1.0f - (hsb.s * F / 64.0));
		int T = B * (1.0f - (hsb.s * (1.0f - F) / 64.0));

		switch ((int)H) {
		case 0:
			return WPixel(B, T, P);
			break;
		case 1:
			return WPixel(Q, B, P);
			break;
		case 2:
			return WPixel(P, B, T);
			break;
		case 3:
			return WPixel(P, Q, B);
			break;
		case 4:
			return WPixel(T, P, B);
			break;
		case 5:
			return WPixel(B, P, Q);
			break;
		default:
			return WPixel(0, 0, 0);
			break;
		}
	}
}



void RLE::RGBtoPixel(WPixel* pixel, int r, int g, int b) {
	*pixel = WPixel(r, g, b);
	/*BYTE* rgb = (BYTE*)pixel;
	*rgb = *rgb & 0xE0 | (b >> 3) & 0x1F;
	*(WORD *)rgb = *(WORD *)rgb & 0xF81F | 32 * ((g >> 2) & 0x3F);
	rgb[1] = rgb[1] & 7 | 8 * ((r >> 3) & 0x1F);*/
}



void RLE::Rle3HSLtoRGB(WPixel* pixel, INT64* entry)
{
	unsigned __int8 cmapEntry[8];
	for (int j = 0; j < 8; j++) cmapEntry[j] = *((unsigned __int8*)entry + j);

	float h = (double)*((unsigned __int16 *)cmapEntry);
	float fHp = h / 60.0;
	signed __int64 v9 = (signed __int64)fHp;
	int H = v9;
	float roundedH = (double)(signed int)v9;
	int c = *(cmapEntry + 2);
	float v13 = fHp - roundedH;
	float dC = (double)c;

	float sat = *(float*)(cmapEntry + 4);
	int z = (signed __int64)((1.0 - sat) * dC);
	int x1 = (signed __int64)((1.0 - v13 * sat) * dC);
	signed __int64 x2 = (signed __int64)((1.0 - (1.0 - v13) * sat) * dC);

	switch (H)
	{
	case 0:
		Rle3ConvertTo16BitRGB(pixel, c, x2, z);
		break;
	case 1:
		Rle3ConvertTo16BitRGB(pixel, x1, c, z);
		break;
	case 2:
		Rle3ConvertTo16BitRGB(pixel, z, c, x2);
		break;
	case 3:
		Rle3ConvertTo16BitRGB(pixel, z, x1, c);
		break;
	case 4:
		Rle3ConvertTo16BitRGB(pixel, x2, z, c);
		break;
	case 5:
		Rle3ConvertTo16BitRGB(pixel, c, z, x1);
		break;
	default:
		Rle3ConvertTo16BitRGB(pixel, 0, 0, 0);
		break;
	}
}



void RLE::Rle3ConvertTo16BitRGB(WPixel* pixel, int r, int g, int b) {
	*(WORD *)pixel = ((b >> 3) & 0x1F | 32 * (((unsigned __int16)(r >> 3) << 6) | (g >> 2) & 0x3F));
}


void RLE::writeToPNG() {
	BYTE *bits;
	BYTE alpha;
	int mapPos;
	FIBITMAP *bitmap = FreeImage_Allocate(header->width, header->height, 32);

	for (unsigned y = 0; y < header->height; y++) {
		int row;
		if (flipped) row = (header->height - 1) - y;
		else row = y;
		bits = FreeImage_GetScanLine(bitmap, row);

		for (unsigned x = 0; x < header->width; x++) {
			int pixelPos = (y* header->width) + x;

			ImageData nextPixel = image[pixelPos];

			alpha = nextPixel.alpha;
			mapPos = nextPixel.mapPos;
			//BITMAPINFOHEADER test;
			ColorMap mappedColor = colorMap[mapPos];
			WPixel pixel;
			/*pixel.red = (BYTE)mappedColor.red & 0x1F;
			pixel.green = (BYTE)mappedColor.green & 0x3F;
			pixel.blue = (BYTE)mappedColor.blue & 0x1F;*/
			pixel.red = mappedColor.red;
			pixel.green = mappedColor.green;
			pixel.blue = mappedColor.blue;

			//Expands 16bit RGB into 24 bit
			bits[3] = alpha;
			bits[2] = ((pixel.red << 3) | (pixel.red >> 2));
			bits[1] = ((pixel.green << 2) | (pixel.green >> 4));
			bits[0] = ((pixel.blue << 3) | (pixel.blue >> 2));

			// jump to next pixel
			bits += 4;
		}
	}
	bits = nullptr;

	std::string pngFile = filePath + fileName + ".png";
	FreeImage_Save(FIF_PNG, bitmap, pngFile.c_str(), 0);
	FreeImage_Unload(bitmap);

	return;
}


void RLE::createBitmap() {
	int start = SDL_GetTicks();

	//pixels = new BYTE[header->width * header->height * 4];

	FIBITMAP *fiBitmap = FreeImage_Allocate(header->width, header->height, 32);
	
	BYTE *bits = nullptr;
	BYTE alpha;
	int mapPos;
	for (unsigned y = 0; y < header->height; y++) {
		int row;
		if (flipped) row = (header->height - 1) - y;
		else row = y;

		bits = FreeImage_GetScanLine(fiBitmap, row);
		//bits = (pixels + (row*y));

		for (unsigned x = 0; x < header->width; x++) {
			int pixelPos = (y * header->width) + x;

			ImageData nextPixel = image[pixelPos];

			alpha = nextPixel.alpha;
			mapPos = nextPixel.mapPos;

			ColorMap mappedColor = colorMap[mapPos];
			WPixel pixel;
			pixel.red = mappedColor.red;
			pixel.green = mappedColor.green;
			pixel.blue = mappedColor.blue;

			//Expands 16bit RGB into 24 bit
			bits[3] = alpha;
			bits[2] = ((pixel.red << 3) | (pixel.red >> 2));
			bits[1] = ((pixel.green << 2) | (pixel.green >> 4));
			bits[0] = ((pixel.blue << 3) | (pixel.blue >> 2));

			// jump to next pixel
			bits += 4;
		}
	}
	bits = nullptr;

	FIMEMORY *destStream = FreeImage_OpenMemory();
	FreeImage_SaveToMemory(FIF_BMP, fiBitmap, destStream, BMP_DEFAULT);
	FreeImage_Unload(fiBitmap);

	bitmapSize = 0;
	unsigned char *mem_buffer = NULL;
	if (!FreeImage_AcquireMemory(destStream, &mem_buffer, (DWORD*)&bitmapSize))
		std::cout << "Error aquiring compressed image from memory" << std::endl;

	if (bitmap) delete[] bitmap;
	bitmap = new BYTE[bitmapSize];
	memcpy(bitmap, mem_buffer, bitmapSize);

	/*std::ofstream ofs("test.bmp", std::ofstream::binary);
	ofs.write((char*)bitmap, bitmapSize);
	ofs.close();*/

	FreeImage_CloseMemory(destStream);

	float elapsed = (SDL_GetTicks() - start) / 10000.0;
	//std::cout << "RLE Bitmap > Memory Bitmap: " << elapsed << std::endl;
}


void RLE::addHsbSets(ColorShifts shifts) {
	for (auto shift : shifts) {
		addHSBShift(shift.hue, shift.newHue, shift.range, shift.sat, shift.bright);
	}
}

void RLE::setColorShifts(ColorShifts shifts) {
	colorShifts.clear();
	for (auto shift : shifts) {
		addHSBShift(shift.hue, shift.newHue, shift.range, shift.sat, shift.bright);
	}
}


void RLE::addHSBShift(int hue, int newHue, int range, int saturation, int light) {
	ColorShift newShift{ hue, newHue, range, saturation, light };

	if (!newRle) {
		if (newShift.sat > 63) newShift.sat = 63;
		if (newShift.sat < 0) newShift.sat = 0;
		if (newShift.bright > 63) newShift.bright = 63;
		if (newShift.bright < 0) newShift.bright = 0;
		if (newShift.newHue > 0xBFu) newShift.newHue = -65;
		if ((signed int)newShift.newHue < 0) newShift.newHue = 0;
	}

	if (newShift.range == 0) {
		return;
	}

	colorShifts.push_back(newShift);
}


void RLE::addHueShift(BYTE newHue) {
	hueShift = newHue;
}


void RLE::reloadColorMap(bool useHslShifts) {
	int start = SDL_GetTicks();

	int currentMapKey = getCurrentMapKey();
	if (lastMapKey == currentMapKey) return;
	lastMapKey = currentMapKey;

	for (int i = 0; i < 256; i++) {
		HSB mapColor = *(((HSB*)bColorMap) + i);
		HSB newColor = mapColor;

		if (useHslShifts) {
			WPixel pixel;
			if (newRle) {
				//New Hue shift applied to hue range of 'sat' and 'light', utilizing existing table cols
				INT64 cmapEntry = *(((INT64*)bColorMap) + i);
				int hue = *((INT16*)&cmapEntry);

				for (auto shift : colorShifts) {
					if (shift.sat <= hue && hue <= shift.bright) {
						hue += shift.newHue;
						if (hue > 359) hue -= 360;
						*((INT16*)&cmapEntry) = hue;
						break;
					}
				}

				Rle3HSLtoRGB(&pixel, &cmapEntry);

				colorMap[i].red = pixel.red;
				colorMap[i].green = pixel.green;
				colorMap[i].blue = pixel.blue;
			}
			else {
				//Loop through HSL Color Shift sets until we find a matching set
				bool hslShiftFound = false;
				for (auto shift : colorShifts) {
					if (shiftHSB(&mapColor, shift)) {
						hslShiftFound = true;
						break;
					}
				}
				if (!hslShiftFound) continue;

				//The new color uses the shifted hue and static sat, brightness changes are done after
				newColor.hue = mapColor.hue;
				newColor.sat = mapColor.sat;
				pixel = HSBtoRGB(newColor);

				//Apply bright shift here
				if (mapColor.bright) {
					int shiftLight;
					if (mapColor.bright) shiftLight = mapColor.bright - 32;
					else shiftLight = 0;
					if (shiftLight > 63) shiftLight = 63;

					//int xRed = 8 * pixel.red;
					int xRed = ((pixel.red << 3) | (pixel.red >> 2));
					xRed += (xRed * 4 * shiftLight) / 100;					
					if (xRed < 0) xRed = 0;
					if (xRed > 252) xRed = 252;

					//int xGreen = 4 * pixel.green;
					int xGreen = ((pixel.green << 2) | (pixel.green >> 4));
					xGreen += (xGreen * 4 * shiftLight) / 100;
					if (xGreen < 0) xGreen = 0;
					if (xGreen > 252) xGreen = 252;

					int xBlue = ((pixel.blue << 3) | (pixel.blue >> 2));
					//int xBlue = 8 * pixel.blue;
					xBlue += (xBlue * 4 * shiftLight) / 100;
					if (xBlue < 0) xBlue = 0;
					if (xBlue > 252) xBlue = 252;

					pixel = WPixel(xRed, xGreen, xBlue);
				}
				colorMap[i].blue = pixel.blue;
				colorMap[i].green = pixel.green;
				colorMap[i].red = pixel.red;
			}
		}
		else {
			//Reload the pixel
			WPixel pixel;
			if (newRle) {
				INT64 cmapEntry = *(((INT64*)bColorMap) + i);
				Rle3HSLtoRGB(&pixel, &cmapEntry);

				colorMap[i].red = pixel.red;
				colorMap[i].green = pixel.green;
				colorMap[i].blue = pixel.blue;
			}
			else {
				pixel = HSBtoRGB(mapColor);
				colorMap[i].blue = pixel.blue;
				colorMap[i].green = pixel.green;
				colorMap[i].red = pixel.red;
			}
		}
	}

	float elapsed = (SDL_GetTicks() - start) / 10000.0;
	//if (useHslShifts)  std::cout << "Color Map Load Time (HSL Shift): " << elapsed << std::endl;
	//else std::cout << "Color Map Load Time: " << elapsed << std::endl;
}


bool RLE::shiftHSB(HSB* hsb, ColorShift colorShift) {
	int minHue, maxHue;
	minHue = colorShift.hue - (colorShift.range >> 1);
	maxHue = (colorShift.range >> 1) + colorShift.hue;

	bool doColorChange = false;
	if ((unsigned __int8)hsb->hue >= minHue && (unsigned __int8)hsb->hue < maxHue) doColorChange = true;
	if (minHue < 0 && (unsigned __int8)hsb->hue > minHue + 192) doColorChange = true;
	if (maxHue >= 192 && (unsigned __int8)hsb->hue < maxHue - 192) doColorChange = true;
	if (!doColorChange) return false;

	int newHue;
	newHue = (unsigned __int8)colorShift.newHue + (unsigned __int8)hsb->hue - (unsigned __int8)colorShift.hue;
	if (newHue >= 192) 	newHue -= 192;
	newHue += newHue < 0 ? 0xC0 : 0;
	hsb->hue = newHue;
	hsb->sat = colorShift.sat;
	hsb->bright = colorShift.bright;

	return true;
}

int RLE::getCurrentMapKey() {
	int mapKey = 0;

	for (auto shift : colorShifts) {
		mapKey += ((shift.hue << 24) | (shift.newHue << 16) | (shift.sat << 8) | shift.sat);
	}

	return mapKey;
}