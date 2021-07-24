#ifndef __INI_H
#define __INI_H

#include <SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

typedef struct INIEntry {
	std::string name;
	std::string value;
} INIEntry;

typedef struct INISection {
	std::string section;
	std::vector<INIEntry> entries;
	~INISection() {
		entries.clear();
	}
} INISection;

class INI {
private:
	std::string fileName;

public:
	std::vector<INISection> sections;
	int currentSection = -1;

	INI(std::string file);
	INI(std::string file, std::string aSection);
	~INI();

	void INI::writeToFile();
	void INI::addSection(std::string name);
	void INI::addEntry(std::string name, std::string value);
	void INI::setEntry(std::string name, std::string value);

	std::vector<std::string> INI::getSections();
	bool INI::setSection(std::string aSection);
	std::string INI::getEntry(std::string aEntry);
	bool INI::getEntry(std::string aEntry, std::string *value);
};

#endif
