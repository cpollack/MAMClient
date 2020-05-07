#pragma once

#include "Entity.h"

enum { MALE, FEMALE };

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
	void setLeaving(bool leaving);
	bool leaveMap();
	bool getJumping();
	bool getWalking();

protected:
	bool walking = false, jumping = false, isLeavingMap = false;
	std::vector<SDL_Point> path;
	SDL_Point DestCoord;
	int lastMoveTick;
	const int WALK_SPEED = 200;
	const int JUMP_SPEED = 350;

	bool atDestCoord();
	void getNextDestCoord();
	virtual void takeNextStep();
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
};