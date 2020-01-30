#pragma once

class Options {
public:
	Options();
	~Options();

private:
	void loadColorDisable();

private:
	bool colorDisable[256];

public:
	bool IsColorDisabled(int role);
};

extern Options options;