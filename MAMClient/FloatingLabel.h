#pragma once

class FloatingLabel {
public:
	FloatingLabel() {}
	~FloatingLabel() {}

	Asset text;
	std::string label;
	//SDL_Point startingPoint;
	//SDL_Point position;
	bool started = false;
	int startTime = 0;
};