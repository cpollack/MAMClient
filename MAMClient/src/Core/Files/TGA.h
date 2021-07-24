#ifndef __TGA_H
#define __TGA_H

#include "FreeImagePlus.h"

class TGA {
	BYTE* bitmap = nullptr;
	int bitmapSize, tgaSize, width, height;
	fi_handle fih;

	//unsigned _stdcall TGA::_ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle);
	//long _stdcall _TellProc(fi_handle handle);
public:
	TGA::TGA(std::string file, unsigned char *wdfBuffer, int size);
	TGA::~TGA();
	BYTE* TGA::getBitmap();
	int TGA::getBitmapSize();
};

#endif
