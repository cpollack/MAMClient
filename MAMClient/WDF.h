#ifndef __WDF_H
#define __WDF_H

#include <windows.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>

typedef struct WDF_Entry {
	DWORD uid;
	const char *name;
	int size;
	int offset;
	unsigned space;
}WDF_Entry;


struct WdfHeader {
	DWORD id;
	int number;
	unsigned offset;
public:
	WdfHeader() : id('WDFP') {};
};


class WDF {
public:
	WDF(std::string wdfPath);
	~WDF();

	int WDF::load();
	void WDF::sortList();
	WDF_Entry* WDF::findEntry(std::string entry);
	void WDF::getFileData(std::string fileName, std::shared_ptr<byte> &buffer, int &size);

	bool WDF::isSorted();

private:
	std::string path;
	WdfHeader header;

	std::vector<WDF_Entry>* unsortedList = nullptr;
	std::vector<WDF_Entry>* sortedList = nullptr;

	unsigned long WDF::stringToId(const char *str);
};

#endif