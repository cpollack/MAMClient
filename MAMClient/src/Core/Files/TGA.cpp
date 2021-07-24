#include "stdafx.h"
#include "TGA.h"


TGA::TGA(std::string file, unsigned char *wdfBuffer, int size) {
	//load the TGA from memory
	FIMEMORY *sourceStream = FreeImage_OpenMemory(wdfBuffer, size);
	FIBITMAP* dib = FreeImage_LoadFromMemory(FIF_TARGA, sourceStream);

	tgaSize = FreeImage_GetDIBSize(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	//Use FI library to generate the bitmap structure
	FIBITMAP *fiBitmap = FreeImage_Allocate(width, height, 32);
	if (!fiBitmap) {
		std::cout << "Failed to FI Allocate in TGA() for file: " << file << std::endl;
		return;
	}

	FIMEMORY *destStream = FreeImage_OpenMemory();

	//copy image data to bitmap here
	int startPos = 0;
	BYTE *destBits, *srcBits;
	BYTE alpha;
	int mapPos;
	bool flipped = false;
	for (unsigned y = 0; y < height; y++) {
		int row;
		if (flipped) row = (height - 1) - y;
		else row = y;

		srcBits = FreeImage_GetScanLine(dib, row);
		destBits = FreeImage_GetScanLine(fiBitmap, row);

		for (unsigned x = 0; x < width; x++) {
			destBits[3] = srcBits[3];
			destBits[2] = srcBits[2];
			destBits[1] = srcBits[1];
			destBits[0] = srcBits[0];

			// jump to next pixel
			srcBits += 4;
			destBits += 4;
		}
	}
	srcBits = nullptr;
	destBits = nullptr;

	//save FI bitmap and write it to bitmap buffer
	FreeImage_SaveToMemory(FIF_BMP, fiBitmap, destStream, 0);
	bitmapSize = 0;
	unsigned char *mem_buffer = NULL;
	if (!FreeImage_AcquireMemory(destStream, &mem_buffer, (DWORD*)&bitmapSize))
		std::cout << "Error aquiring compressed image from memory" << std::endl;

	bitmap = new BYTE[bitmapSize];
	memcpy(bitmap, mem_buffer, bitmapSize);
	FreeImage_Unload(fiBitmap);
	FreeImage_CloseMemory(destStream);

	FreeImage_Unload(dib);
}


TGA::~TGA() {
	delete[] bitmap;
}


BYTE* TGA::getBitmap() {
	return bitmap;
}


int TGA::getBitmapSize() {
	return bitmapSize;
}