#pragma once

extern Uint32 FIRSTCUSTOMEVENT;
extern Uint32 CUSTOMEVENT_WINDOW;
enum {
	WINDOW_CLOSE,
	WINDOW_CLOSE_OK,
	WINDOW_CLOSE_BACK,
	WINDOW_CLOSE_PROMPT_OK,
	WINDOW_CLOSE_PROMPT_ERROR
};

extern Uint32 CUSTOMEVENT_WIDGET;
enum {
	WIDGET_FOCUS_GAINED,
	WIDGET_FOCUS_LOST
};

extern Uint32 CUSTOMEVENT_BATTLE;
enum {
	BATTLE_START,
	BATTLE_END,
};

extern Uint32 CUSTOMEVENT_PLAYER;
enum {
	PLAYER_NONE,
	PLAYER_RENAME,
	PLAYER_UPDATE,
	PLAYER_UPDATEDETAILS,
	PLAYER_LIFE,
	PLAYER_LIFEMAX,
	PLAYER_MANA,
	PLAYER_MANAMAX,
	PLAYER_LIFEMANA,
	PLAYER_EXP,
	PLAYER_LEVEL,
	PLAYER_MONEY,
	PLAYER_REPUTATION,
	PLAYER_LOOK,
	PLAYER_TEAM,
};

extern Uint32 CUSTOMEVENT_PET;
enum {
	PET_NONE,
	PET_MARCHING,
	PET_LIFE,
	PET_LIFEMAX,
	PET_EXP,
	PET_LEVEL,
};

extern Uint32 CUSTOMEVENT_USER;
enum {
	USER_RIGHTCLICK,
};

extern Uint32 CUSTOMEVENT_NPC;
enum {
	NPC_NONE,
	NPC_INTERACT,
	NPC_SHOP
};

extern Uint32 CUSTOMEVENT_ITEM;
enum {
	ITEM_NONE,
	ITEM_USE,
	ITEM_DROP,
	ITEM_BUY,
	ITEM_SELL,
};