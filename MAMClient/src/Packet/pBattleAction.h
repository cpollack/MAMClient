#ifndef __PBATTLEACTION_H
#define __PBATTLEACTION_H
#include "Packet.h"


typedef enum BattleAction {
	baAttack = 0,
	baSpecial = 1,
	baDefend = 2,
	baNoAction = 3,
	//baUnknown = 4, //call pet?
	baPlayerRun = 5,
	baCapture = 6,
	baCaptureFail = 7,
	baCaptureSuccess = 8,
	baUseItem = 9,
	//baUnknown = 11
}BattleAction;


class pBattleAction : public Packet {
public:
	int action, round;
	int sourceId, targetId;
	int useId;
	int verification;

	pBattleAction(char *buf, char* encBuf);
	pBattleAction(int act, int rnd, int srcId, int trgId, int usId, int accountId);
	~pBattleAction();

	virtual void process();
	void pBattleAction::debugPrint();
};

#endif