#pragma once

class Sprite;

class Entity {
public:
	Entity(SDL_Renderer* r, int id, std::string name, int look);
	virtual ~Entity();

	virtual void render();
	virtual void renderNameplate();
	virtual void step();
	virtual void handleEvent(SDL_Event& e);

protected:
	SDL_Renderer* renderer;
	int ID;
	std::string Name;
	SDL_Point Coord, Position, RenderPos;
	
	int Look, Face;
	std::string Role;
	int Animation;
	Sprite* sprite = nullptr;
	int Direction, sprDirection;
	BYTE colorSets[35];
	int lastSpriteAnimation = -1, lastSpriteDirection = -1;

	bool MouseOver = false;

public: 
	int getID();
	std::string getName();
	Sprite* GetSprite() { return sprite; }
	SDL_Rect getRenderRect();

	virtual std::string getRole(int look);
	virtual void setRole(int look);
	int GetLook() { return Look;  }
	int GetFace() { return Face;  }

	virtual void setCoord(SDL_Point coord);
	SDL_Point getCoord();

	void setDirection(int direction);
	void setDirectionToCoord(SDL_Point coordinate);
	int getDirection();
	int getDirectionToCoord(SDL_Point coordinate);

	void setAnimation(int animation);
	virtual void loadSprite();
};