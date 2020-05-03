#include "stdafx.h"
#include "INI.h"


INI::INI(std::string file) {
	fileName = file;
	int start = SDL_GetTicks();
	std::ifstream ifs(file);
	if (ifs) {
		while (!ifs.eof()) {
			std::string line;
			std::getline(ifs, line);

			if (line.empty()) continue;

			if (line.at(0) == '[') {
				INISection aSection;
				aSection.section = line.substr(1, line.find(']') - 1);

				std::getline(ifs, line);
				while (!ifs.eof() && !line.empty()) {
					INIEntry aEntry;
					int posEq = line.find('=');
					
					aEntry.name = line.substr(0, posEq);
					aEntry.value = line.substr(posEq+1, std::string::npos);
					aSection.entries.push_back(aEntry);

					std::getline(ifs, line);
				}
				sections.push_back(aSection);
			}
		}
	}
	ifs.close();

	float time = (SDL_GetTicks() - start) / 1000.0;
	std::cout << "INI Full load time for '" << file << "': " << time << "ms\n";
}


INI::INI(std::string file, std::string aSection) {
	fileName = file;
	int start = SDL_GetTicks();
	std::ifstream ifs(file);
	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	if (ifs) {
		char * buffer = new char[length];
		ifs.read(buffer, length);

		int pos = -1;
		int sectionStartPos = -1, entryStartPos = -1;
		bool sectionFound = false, getNextLine = false;

		INISection iSection;

		while (pos < length) {
			pos++;

			if (getNextLine) {
				if (buffer[pos] == '\n') getNextLine = false;
				continue;
			}

			if (sectionFound) {
				if (entryStartPos == -1) {
					if (buffer[pos] == '\n') continue;
					if (buffer[pos] == '[') {
						break;
					}
					else {
						entryStartPos = pos;
						continue;
					}
				}
				else {
					if (buffer[pos] == '\n') {
						int len = pos - entryStartPos;
						char* tmp = new char[len];
						memcpy(tmp, buffer + entryStartPos, len);
						std::string newEntry(tmp, len);
						delete[] tmp;
						entryStartPos = -1;

						INIEntry iEntry;
						int posEq = newEntry.find('=');

						iEntry.name = newEntry.substr(0, posEq);
						iEntry.value = newEntry.substr(posEq + 1, std::string::npos);
						iSection.entries.push_back(iEntry);
					}
				}
			}

			if (sectionStartPos != -1) {
				if (buffer[pos] == ']') {
					int len = pos - sectionStartPos - 1;
					char* tmp = new char[len];
					memcpy(tmp, buffer + sectionStartPos + 1, len);
					std::string sectionName(tmp, len);
					delete[] tmp;
					if (sectionName == aSection) {
						sectionFound = true;
						iSection.section = sectionName;
					}
					getNextLine = true;
					sectionStartPos = -1;
					continue;
				}
			}

			if (buffer[pos] == '[') {
				sectionStartPos = pos;
				continue;
			}
		}

		if (sectionFound) {
			sections.push_back(iSection);
			currentSection = 0;
		}
		delete[] buffer;
	}
	ifs.close();

	int time = (SDL_GetTicks() - start) / 1000.0;
	//std::cout << "Optimzed INI Sectional load time for '" << file << "': " << time << "ms\n";
}

INI::~INI() {
	sections.clear();
}


void INI::writeToFile() {
	std::ofstream ofs(fileName);
	if (ofs) {
		for (auto section : sections) {
			ofs << "[" << section.section << "]" << std::endl;
			for (auto entry : section.entries) {
				ofs << entry.name << "=" << entry.value << std::endl;
			}
			ofs << std::endl;
		}
	}
}


void INI::addSection(std::string name) {
	INISection newSection;
	newSection.section = name;
	sections.push_back(newSection);
}


void INI::addEntry(std::string name, std::string value) {
	if (currentSection >= 0) {
		INIEntry newEntry;
		newEntry.name = name;
		newEntry.value = value;
		sections.at(currentSection).entries.push_back(newEntry);
	}
}


void INI::setEntry(std::string name, std::string value) {
	if (currentSection == -1) {
		std::cout << "INI Error: " << fileName << " - section not loaded";
		return;
	}

	std::vector<INIEntry> *entries = &sections.at(currentSection).entries;
	for (int i = 0; i < entries->size(); i++) {
		if (entries->at(i).name.compare(name) == 0) {
			entries->at(i).value = value;
			return;
		}
	}

	addEntry(name, value);
}


std::vector<std::string> INI::getSections() {
	std::vector<std::string> strSections;
	for (int i = 0; i < sections.size(); i++) {
		strSections.push_back(sections.at(i).section);
	}
	return strSections;
}


bool INI::setSection(std::string aSection) {
	currentSection = -1;
	for (int i = 0; i < sections.size(); i++) {
		if (sections.at(i).section.compare(aSection) == 0) {
			currentSection = i; 
			return true;
		}
	}
	return false;
}


std::string INI::getEntry(std::string aEntry) {
	if (currentSection == -1) {
		std::cout << "INI Error: " << fileName << " - section not loaded";
		return "";
	}

	std::vector<INIEntry> entries = sections.at(currentSection).entries;
	for (int i = 0; i < entries.size(); i++) {
		if (entries.at(i).name.compare(aEntry) == 0) return entries.at(i).value;
	}
	return "";
}


bool INI::getEntry(std::string aEntry, std::string *value) {
	if (currentSection == -1) {
		std::cout << "INI Error: " << fileName << " - section not loaded";
		return false;
	}

	std::vector<INIEntry> entries = sections.at(currentSection).entries;
	for (int i = 0; i < entries.size(); i++) {
		if (entries.at(i).name.compare(aEntry) == 0) {
			*value = entries.at(i).value;
			return true;
		}
	}
	return false;
}