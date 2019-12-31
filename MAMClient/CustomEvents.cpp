#include "stdafx.h"
#include "CustomEvents.h"

Uint32 CUSTOMEVENT_WINDOW = SDL_RegisterEvents(1);
Uint32 FIRSTCUSTOMEVENT = CUSTOMEVENT_WINDOW;
Uint32 CUSTOMEVENT_WIDGET = SDL_RegisterEvents(1);
Uint32 CUSTOMEVENT_PLAYER = SDL_RegisterEvents(1);