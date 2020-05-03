#pragma once

class Sprite;
class GameObj {
public:
	GameObj(SDL_Renderer* r);
	~GameObj() {}

	virtual void render();
	virtual void step() {}
	virtual void handleEvent(SDL_Event& e) {}

protected:
	SDL_Renderer* renderer;
	Sprite* sprite = nullptr;

	int Type;

	SDL_Point Coord, Position, RenderPos;

public:
	void SetRenderer(SDL_Renderer* rend) { renderer = rend; }

	virtual Sprite* GetSprite(bool forBattle = false) { return sprite; } //overload for battle usage, to be fixed by CFighter
	void SetSprite(Sprite* spr) { sprite = spr; }

	int GetType() { return Type; }
	void SetType(int iType) { Type = iType; }

	SDL_Point GetCoord() { return Coord; }
	virtual void SetCoord(SDL_Point coord);

	SDL_Point GetPosition() { return Position; }
	void SetPosition(SDL_Point pos) { Position = pos; }

	virtual SDL_Rect GetRenderRect();
	virtual SDL_Rect GetRenderRect(int frame);
};