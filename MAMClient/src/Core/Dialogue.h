#pragma once

#include "Widget.h"

class Texture;
class pNpcDialogue;
class Dialogue : public CWidget {
public:
	int portraitId;
	std::string name, dialogue, dialogue2, response1, response2, response3;
	int selection = -1;

	Dialogue(CWindow* window, pNpcDialogue* packet, std::string npcName, int pX, int pY);
	~Dialogue();

	void Render();
	void HandleEvent(SDL_Event& e);
	void setWindowOffset(SDL_Point p);

private:
	const int MINHEIGHT = 125;
	const int MESSAGE_WRAP = 430;
	const int RESPONSE_WRAP = 200;

	SDL_Point windowOffset;
	SDL_Texture* mainTexture;
	Texture* portrait;
	bool boxClicked;

	std::vector<std::string> messages;
	std::vector<std::string> responses;
	int focusedOption = -1;
	SDL_Rect exitRect;
	SDL_Rect respRect[4];

	void Dialogue::createDialogueBox();
	Texture* createDialogueText(std::string text, int maxWidth, SDL_Color color);
	void Dialogue::stripSquiggly(std::string* str);
};

extern Dialogue *dialogue;