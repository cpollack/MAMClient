#pragma once

#include "GameObj.h"
#include "Define.h"

class Sprite;
class FloatingLabel;

class Entity : public GameObj {
public:
	Entity(SDL_Renderer* r, int id, std::string name, int look);
	virtual ~Entity();

	virtual void CleanupBattle();

	virtual void render();
	virtual void render_battle();
	virtual void render_effects(SDL_Point point);
	virtual void render_floatingLabels();
	virtual void render_nameplate();
	virtual void step();
	virtual void handleEvent(SDL_Event& e);
	virtual void handleEvent_battle(SDL_Event& e);

protected:
	int ID;
	std::string Name;
	SDL_Point BattlePos_Base, BattlePos, TargetingPos;
	
	int Look, Face;
	std::string Role;
	int Animation;
	int lastSpriteAnimation = -1, lastSpriteDirection = -1;
	ColorShifts colorShifts;

	std::vector<Effect> effects;

	int Direction, sprDirection;

	bool NameplateBackground = false;
	bool MouseOver = false;

public:
	int GetID() { return ID; }

	std::string GetName() { return Name; }
	void SetName(std::string name) { Name = name; }

	virtual Sprite* GetSprite(bool forBattle = false) { return !forBattle ? sprite : BattleSprite; }

	SDL_Rect GetRenderRect(bool forBattle = false);
	SDL_Rect GetRenderRect(int frame, bool forBattle = false);

	static std::string getRole(int look);
	void setRole(int look);

	int GetLook() { return Look; }
	virtual void SetLook(int iLook);

	int GetFace() { return Face; }

	virtual void setDirection(int direction, bool forBattle = false);
	void setDirectionToCoord(SDL_Point coordinate, bool forBattle = false);
	int getDirection(bool forBattle = false);
	int getDirectionToCoord(SDL_Point coordinate);

	void SetEmotion(int emotion);
	void setAnimation(int animation, bool forBattle = false);
	void setColorShifts(ColorShifts shifts, bool forBattle = false);
	virtual void loadSprite(bool forBattle = false);

	bool IsInvisible() { return Invisible; }
	void SetInvisible(bool bInvis) { Invisible = bInvis; }

	bool IsMousedOver() { return MouseOver; }

	Sprite* CreateEffectSprite(int effect);
	Sprite* addEffect(int effect);
	Sprite* findEffect(int effect);
	std::vector<Effect>::iterator removeEffect(int effect);
	std::vector<Effect>::iterator removeEffect(Sprite* effect);
	void clearEffects();

protected: //Attributes
	int Level = 0;
	int life_current, life_max, mana_current, mana_max;

public: //Attributes
	int GetLevel() { return Level; }
	void SetLevel(int iLevel) { Level = iLevel; }

	int GetCurrentLife() { return life_current; }
	virtual int GetMaxLife() { return life_max; }
	void SetLife(int iLife) { life_current = iLife; if (life_current < 0) life_current = 0; if (life_current > life_max) life_current = life_max; }
	void SetMaxLife(int iLife) { life_max = iLife; }

	int GetCurrentMana() { return mana_current; }
	int GetMaxMana() { return mana_max; }
	void SetMana(int iMana) { mana_current = iMana; if (mana_current < 0) mana_current = 0; if (mana_current > mana_max) mana_current = mana_max; }
	void SetMaxMana(int iMana) { mana_max = iMana; }

protected: // Battle
	Sprite* BattleSprite = nullptr;
	ColorShifts BattleColorShifts;
	int BattleAnimation, lastBattleAnimation;
	int BattleSprDirection, lastBattleDirection;

	int BattleID;
	int BattleType = 0;
	int ArrayPos = -1;
	bool alive = true;
	bool defending = false;
	std::string BattleYell = "";
	std::vector<FloatingLabel> floatingLabels;

	bool Invisible = false; //Treat as if it is within renderable area, but do not render main Sprite. Useful for effects IE Sphere/Mirror

public:
	int GetBattleId() { return BattleID; }

	int GetBattleType() { return BattleType; }
	void SetBattleType(int iType) { BattleType = iType; }

	SDL_Point GetBattlePos() { return BattlePos; }
	void SetBattlePos(SDL_Point pos) { BattlePos = pos; }

	SDL_Point GetBattleBasePos() { return BattlePos_Base; }
	void SetBattleBasePos(SDL_Point pos) { BattlePos_Base = pos; }

	SDL_Point GetTargetingPos() { return TargetingPos; }
	void SetTargetingPos(SDL_Point pos) { TargetingPos = pos; }

	int GetAyrayPos() { return ArrayPos; }
	void SetArrayPos(int pos) { ArrayPos = pos; }

	std::string GetBattleYell() { return BattleYell; }
	void SetBattleYell(std::string yell) { BattleYell = yell; }
	
	void SetAlive(bool bAlive) { alive = bAlive; }
	void SetDefending(bool bDefend) { defending = bDefend; }

	bool IsAlive() { return alive; }
	bool IsDefending() { return defending; }
	bool IsEnemy();

	void addFloatingLabel(std::string text);
};