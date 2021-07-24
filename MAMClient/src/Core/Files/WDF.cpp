#include "stdafx.h"
#include "WDF.h"

WDF::WDF(std::string wdfPath) {
	path = wdfPath;
	unsortedList = new std::vector<WDF_Entry>;
}


WDF::~WDF() {
	if (unsortedList) {
		unsortedList->clear();
		delete unsortedList;
	}

	if (sortedList) {
		sortedList->clear();
		delete sortedList;
	}
}

int WDF::load() {
	FILE *f;
	fopen_s(&f, path.c_str(), "rb");
	if (f == 0) {
		return -1;
	}

	fread(&header, sizeof(header), 1, f);
	if (header.id != 'WDFP') {
		return -1;
	}

	unsortedList->resize(0);

	fseek(f, header.offset, SEEK_SET);
	for (int i = 0; i < header.number; i++) {
		WDF_Entry entry;

		fread(&(entry.uid), 4, 1, f);
		fread(&(entry.offset), 4, 1, f);
		fread(&(entry.size), 4, 1, f);
		fread(&(entry.space), 4, 1, f);

		unsortedList->push_back(entry);
	}
	fclose(f);
	return header.number;
}


void WDF::sortList() {
	int startTime = GetTickCount();
	sortedList = new std::vector<WDF_Entry>;
	
	int count = 0;
	for (auto item : *unsortedList) {
		WDF_Entry nextItem = item;

		int l, r;
		int atPos;
		l = 0;
		r = sortedList->size() - 1;
		if (r == -1) {
			sortedList->insert(sortedList->begin() + 0, nextItem);
			continue;
		}

		while (true) {
			int newPos = l + ((r - l) / 2);

			if (l == r || l > r) {
				if (nextItem.uid < sortedList->at(newPos).uid) sortedList->insert(sortedList->begin() + newPos, nextItem);
				else sortedList->insert(sortedList->begin() + newPos + 1, nextItem);
				break;
			}

			if (nextItem.uid < sortedList->at(newPos).uid) {
				r = newPos - 1;
				continue;
			}

			if (nextItem.uid > sortedList->at(newPos).uid) {
				l = newPos + 1;
				continue;
			}
		}
	}
	float currentTime = (GetTickCount() - startTime) / 1000.0;
	//std::cout << "WDF Sort time: " << std::to_string(currentTime) << std::endl;
}


WDF_Entry* WDF::findEntry(std::string entry) {
	//format string - all lower and /
	for (int i = 0; i < entry.size(); i++) if (entry[i] == '\\') entry[i] = '/';
	transform(entry.begin(), entry.end(), entry.begin(), ::tolower);

	unsigned long fileId = stringToId(entry.c_str());

	if (sortedList) {
		int l = 0;
		int r = sortedList->size() - 1;
		while (true) {
			int newPos = l + ((r - l) / 2);
			if (fileId == sortedList->at(newPos).uid) {
				return &sortedList->at(newPos);
			}

			if (l == r || l > r) break;

			if (fileId < sortedList->at(newPos).uid) {
				r = newPos - 1;
				continue;
			}

			if (fileId > sortedList->at(newPos).uid) {
				l = newPos + 1;
				continue;
			}
		}
	}
	else {
		for (int i = 0; i < unsortedList->size(); i++) {
			if (fileId == unsortedList->at(i).uid) return &unsortedList->at(i);
		}
	}

	return nullptr;
}


unsigned long WDF::stringToId(const char *str) {
	int i;
	unsigned int v;
	static unsigned m[70];
	strncpy((char *)m, str, 256);
	for (i = 0; i<256 / 4 && m[i]; i++);
	m[i++] = 0x9BE74448, m[i++] = 0x66F42C48;
	v = 0xF4FA8928;

	__asm {
		mov esi, 0x37A8470E; x0 = 0x37A8470E
		mov edi, 0x7758B42B; y0 = 0x7758B42B
		xor ecx, ecx
		_loop :
		mov ebx, 0x267B0B11; w = 0x267B0B11
			rol v, 1
			lea eax, m
			xor ebx, v

			mov eax, [eax + ecx * 4]
			mov edx, ebx
			xor esi, eax
			xor edi, eax

			add edx, edi
			or edx, 0x2040801; a = 0x2040801
			and edx, 0xBFEF7FDF; c = 0xBFEF7FDF

			mov eax, esi
			mul edx
			adc eax, edx
			mov edx, ebx
			adc eax, 0

			add edx, esi
			or edx, 0x804021; b = 0x804021
			and edx, 0x7DFEFBFF; d = 0x7DFEFBFF

			mov esi, eax
			mov eax, edi
			mul edx

			add edx, edx
			adc eax, edx
			jnc _skip
			add eax, 2
			_skip:
		inc ecx;
		mov edi, eax
			cmp ecx, i
			jnz _loop
			xor esi, edi
			mov v, esi
	}

	return v;
}


void WDF::getFileData(std::string fileName, std::shared_ptr<DataBuffer> &buffer, int &size) {
	WDF_Entry* wdfEntry = findEntry(fileName);
	if (!wdfEntry) return;

	buffer.reset(new DataBuffer(wdfEntry->size));
	size = wdfEntry->size;

	std::ifstream wdf_ifs(path, std::ios::binary | std::ios::ate);
	wdf_ifs.seekg(wdfEntry->offset, std::ios::beg);
	wdf_ifs.read((char*)buffer.get()->buffer, wdfEntry->size);
	wdf_ifs.close();
}


bool WDF::isSorted() {
	if (sortedList) return true;
	return false;
}