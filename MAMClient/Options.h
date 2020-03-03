#pragma once

class Options {
public:
	Options();
	~Options();

private:
	void loadColorDisable();

private:
	bool colorDisable[256];
	bool autoBattle = false;

public:
	bool IsColorDisabled(int role);
	
	void SetAutoBattle(bool bAuto) { autoBattle = bAuto; }
	bool GetAutoBattle() { return autoBattle; }
};

extern Options options;