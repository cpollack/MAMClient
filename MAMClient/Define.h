#pragma once

enum HorizontalAlignment {
	haLeft,
	haCenter,
	haRight
};

enum VerticalAlignment {
	vaTop,
	vaCenter,
	vaBottom
};

//Battle Info
const DWORD	_IDMSK_PET = 0x80000000;
const DWORD _IDMSK_MONSTER = 0xc0000000;

enum { OBJ_NONE = 1234, OBJ_MONSTER, OBJ_PET, OBJ_USER, OBJ_ITEM };

enum { OBJTYPE_MONSTER = 0, OBJTYPE_VSPET, OBJTYPE_VSPLAYER, OBJTYPE_FRIENDPET, OBJTYPE_FRIENDPLAYER };