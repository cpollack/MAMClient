#pragma once

#include "Entity.h"

class pUserInfo;
class Sprite;

enum { MALE, FEMALE };

class User : public Entity {
public:
	User(pUserInfo *packet);
	User(SDL_Renderer* r, int id, std::string name, int look);
	~User();

	virtual void render();
	virtual void renderNameplate();
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

	bool atDestCoord();
	void getNextDestCoord();
	virtual void takeNextStep();

protected:
	std::string NickName;
	std::string Spouse;
	int Level;

	int FullRank;
	int Rank;
	int Reborns;
	int RankType;
	int MasterType;
	int Alignment;

	int pkEnabled;
	int SyndicateId, SubGroudId, SyndicateRank;
	std::string Guild, SubGroup, GuildTitle;
	int Gender;

public:
	std::string getNickName();
	std::string getSpouse();
	
	int getLevel();
	int GetRank() { return Rank;  }
	std::string GetRankText();
	int GetGender() { return Gender;  }

	std::string getGuild();
	std::string getGuildTitle();

protected: /* Effects */
	Sprite *effect = nullptr, *aura = nullptr;
	std::string effectName;
	void loadEffect(std::string _effectName);
	void loadAura();
};