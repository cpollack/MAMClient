#include "stdafx.h"
#include "Options.h"

Options options;

Options::Options() {
	loadColorDisable();
}

Options::~Options() {

}

void Options::loadColorDisable() {
	std::string file = "ini/Colordisable.ini";
	std::ifstream ifs(file);

	if (!ifs.is_open()) return;

	while (!ifs.eof()) {
		char buf[4];
		ifs.getline(buf, 4);
		
		try {
			int role = atoi(buf);
			if (role >= 0 && role <= 255) colorDisable[role] = true;
		}
		catch (...) {
			//
		}
	}
}

bool Options::IsColorDisabled(int role) {
	if (role < 0 || role > 255) return false;
	return colorDisable[role];
}