#include "stdafx.h"
#include "Global.h"

#include "Define.h"

bool bExitApplication = false;
SDL_AudioDeviceID audioDevice;

int GameMode = GAMEMODE_NONE;
double AverageTickLength = 0;