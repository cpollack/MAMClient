#pragma once

#include "Widget.h"

class CGauge : public CWidget {
public:
	CGauge(CWindow* window, std::string name, int x, int y);
	CGauge(CWindow* window, rapidjson::Value& vWidget);
	~CGauge();

	void Render();
	void HandleEvent(SDL_Event& e);

	void Render_Textures();
	void Render_Draw();

	void SetForegroundImage(std::string imagePath);
	void SetBackgroundImage(std::string imagePath);
	void SetDecreaseImage(std::string imagePath);
	void SetIncreaseImage(std::string imagePath);
	void SetUseGUI(bool use);

	void set(int val);
	void set(int val, int max);
	void add(int val);
	void subtract(int val);

private:
	void CreateGaugeTexture();
	void updateLabel();

	SDL_Texture *backgroundTexture;
	SDL_Texture *foregroundTexture;
	SDL_Texture *decreaseTexture;
	SDL_Texture *increaseTexture;
	Texture* backgroundImage = nullptr;
	Texture* foregroundImage = nullptr;
	Texture* decreaseImage = nullptr;
	Texture* increaseImage = nullptr;

	bool Loaded = false;
	bool ShowLabel = false;
	bool UseGUI = false;
	bool usingImages = false;
	std::string backgroundImagePath;
	std::string foregroundImagePath;
	std::string decreaseImagePath;
	std::string increaseImagePath;

	int Max;
	int Current;
	float fillPerc;
	int shiftSpeed = 500; //total time in MS to shift to new position
	int shiftDown = -1, shiftUp = -1;
	int startTime, elapsedTime;
};
