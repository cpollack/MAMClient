#pragma once

#include "GameObj.h"
#include "Define.h"

class Sprite;
class FloatingLabel;

class Entity : public GameObj {
public:
	Entity(SDL_Renderer* r, int id, std::string name, int look);
	virtual ~Entity();

	virtual void render();
	virtual void render_effects(SDL_Point point);
	//virtual void render_floatingLabels();
	virtual void render_nameplate();
	virtual void step();
	virtual void handleEvent(SDL_Event& e);

protected:
	int ID;
	std::string Name;
	
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

	virtual Sprite* GetSprite() { return sprite; }

	static std::string getRole(int look);
	void setRole(int look);

	int GetLook() { return Look; }
	virtual void SetLook(int iLook);

	int GetFace() { return Face; }

	virtual void setDirection(int direction);
	void setDirectionToCoord(SDL_Point coordinate);
	int getDirection();
	int getDirectionToCoord(SDL_Point coordinate);

	void SetEmotion(int emotion);
	void setAnimation(int animation);
	void setColorShifts(ColorShifts shifts);
	virtual void loadSprite();

	bool Visible();
	void SetVisible(bool bVisible);

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
};