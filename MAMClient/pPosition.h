#ifndef __PPOSITION_H
#define __PPOSITION_H
#include "Packet.h"

enum PoisitonMode {
	pmNone = 0,
	//1?
	pmJump = 2,
	//3?
	pmDisguise = 4, //Updates a player's look, followed by color packet
	//5 - after setting battle formation
	//6 - restricted to GM or PM?
	//7?
	pmLeave = 8, //Leave map, to be confirmed since xy may not always be 0
	//9 - something you cant do while a teammate
	//10 - happens after battle in IDA
	pmFly = 11, //actTakeoffExecute (ride cloud)
	pmLand = 12, //actLandingExecute (descend cloud)
	//13 - actSuicideExecute (reborn/advance??)
	//14?
	//15 - act rain/snow/stopweather execute
	//16 - pnlScreenMouseDown case 1040
	//17 - actDamageMagExecute
	//18 - actCurMagExecute?
	pmLogin = 19 //Done on first login, where else? get/leaveWord and from main loop.
	//20 - pnlScreenMouseDown case 1044
};

//Is this packet also used to set things for battle? formation, team, etc?

class pPosition : public Packet {
public:
	int userId;
	int value;
	int posX, posY;
	int mode;

	pPosition(char *buf, char* encBuf);
	pPosition(int accountId, int uId, int dir, int pX, int pY, int mode);
	~pPosition();

	virtual void process();
	void pPosition::debugPrint();
};

#endif