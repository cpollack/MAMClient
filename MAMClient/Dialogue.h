#ifndef __DIALOGUE_H
#define __DIALOGUE_H

#include "SDL2_gfxPrimitives.h"
#include "pNpcDialogue.h"
#include "Label.h"

class Dialogue {
public:
	int x, y;
	int portraitId;
	std::string name, dialogue, dialogue2, response1, response2, response3;
	int selection = -1;

	Dialogue(pNpcDialogue* packet, std::string npcName, int pX, int pY);
	~Dialogue();

	void Dialogue::render();
	bool Dialogue::handleEvent(SDL_Event* e);
	void Dialogue::setPosition(int pX, int pY);
	void setWindowOffset(SDL_Point p);
	SDL_Rect Dialogue::getRendeRect();

private:
	const int MINHEIGHT = 125;
	const int MESSAGE_WRAP = 430;
	const int RESPONSE_WRAP = 200;

	int width, height;
	SDL_Point windowOffset;
	SDL_Renderer* renderer;
	SDL_Texture* mainTexture;
	Texture* portrait;
	SDL_Rect renderRect;
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

#endif
