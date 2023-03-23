#pragma once

#include "Entity.h"

enum { MALE, FEMALE };
enum { CLOUD_FLY, CLOUD_LIFT, CLOUD_LAND };

class pUserInfo;
class Sprite;
class CTeam;
class User : public Entity {
public:
	User(pUserInfo *packet);
	User(SDL_Renderer* r, int id, std::string name, int look);
	~User();

	virtual void render();
	virtual void step();
	virtual void handleEvent(SDL_Event& e);

public:
	virtual void jumpTo(SDL_Point coord);
	virtual void walkTo(SDL_Point coord);
	virtual void followPath(std::vector<SDL_Point> path);
	void setLeaving(bool leaving);
	bool leaveMap();
	bool getJumping();
	bool getWalking();

	virtual void setDirection(int direction);

protected:
	bool walking = false, jumping = false, isLeavingMap = false;
	std::vector<SDL_Point> path;
	//SDL_Point DestCoord; Destination coord is always the front of the path
	int lastMoveTick;
	const int WALK_SPEED = 200;
	const int JUMP_SPEED = 350;
	const int FLY_SPEED = 50;

	bool AtDestCoord();
	SDL_Point GetDestCoord();
	virtual void MoveAlongPath();
	void clearPath();

protected:
	std::string NickName;
	std::string Spouse;

	int FullRank;
	int Rank;
	int Reborns;
	int RankType;
	int MasterRank;
	int Alignment;

	int pkEnabled;
	int SyndicateId, SubGroudId, SyndicateRank;
	std::string Guild, SubGroup, GuildTitle;
	int Gender;

public:
	std::string getNickName();
	void SetNickName(std::string newName) { NickName = newName; }

	std::string getSpouse();

	int GetRank() { return Rank;  }
	void SetRank(int iRank) { Rank = iRank; }
	std::string GetRankText();
	bool IsMaster() { return MasterRank > 0; }

	int GetReborns() { return Reborns; }
	void SetReborns(int iReborns) { Reborns = iReborns; }

	int GetAlignment() { return Alignment; }
	void SetAlignment(int iAlign) { Alignment = iAlign; }

	int GetGender() { return Gender;  }

	int GetPK() { return pkEnabled; }
	void SetPK(int iPK) { pkEnabled = iPK; }

	std::string getGuild();
	std::string getGuildTitle();

protected: /* Effects */
	Sprite *Aura = nullptr;
	void loadAura();

	Sprite *TeamLeader = nullptr;
	void loadTeamLead();

public: //Team
	CTeam* GetTeam() { return team; }
	void CreateTeam();
	void JoinTeam(CTeam* pTeam);
	void LeaveTeam();
	bool IsTeamLeader();
protected: 
	CTeam *team = nullptr;

protected: //Clouds
	bool Flying = false, Ascending = false, Descending = false;
	DWORD CloudInitTime = 0;
	Sprite *Cloud = nullptr;
	Asset CloudShadow;
	const int CLOUD_HEIGHT = 80;
	const int CLOUD_LIFT_MS = 2000;

	int cloud_bob = 0;
	bool BOB_UP = true;
	const int MAX_BOB = 5;
	const int BOB_MS = 100;
	DWORD CloudBobTime = 0;
public:
	bool GetFlying() { return Flying; }
	bool IsFlying() { return GetFlying(); }
	void SetFlying(bool bFly) { Flying = bFly; }
	void TakeOff();
	void Land();
	void LoadCloud(int mode);
	void RemoveCloud();
};