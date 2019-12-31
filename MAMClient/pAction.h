#ifndef __PPOSITION_H
#define __PPOSITION_H
#include "Packet.h"

enum ActionMode {
	amNone = 0,
	amDirection = 1, //unconfirmed
	amJump = 2,
	amEmotion = 3, //unconfirmed
	amDisguise = 4, //Updates a player's look, followed by color packet
	//5 - after setting battle formation
	//6 - restricted to GM or PM?
	amReallot = 6, // reallocate distributed stats. action defines 50% or otherwise, but how is it calced it px/py for packet?
	//7?
	amLeave = 8, //Leave map, to be confirmed since xy may not always be 0
	//9 - something you cant do while a teammate
	//10 - happens after battle in IDA
	amFly = 11, //actTakeoffExecute (ride cloud)
	amLand = 12, //actLandingExecute (descend cloud)
	//13 - actSuicideExecute (reborn/advance??)
	//14?
	//15 - act rain/snow/stopweather execute
	//16 - pnlScreenMouseDown case 1040
	//17 - actDamageMagExecute
	//18 - actCurMagExecute?
	amLogin = 19 //Done on first login, where else? get/leaveWord and from main loop.
	//20 - pnlScreenMouseDown case 1044
};

//Is this packet also used to set things for battle? formation, team, etc?

class pAction : public Packet {
public:
	int userId;
	int direction;
	int posX, posY;
	int mode;

	pAction(char *buf, char* encBuf);
	pAction(int accountId, int uId, int dir, int pX, int pY, int mode);
	~pAction();

	virtual void process();
	void pAction::debugPrint();
};

#endif