#include "stdafx.h"
#include "Dialogue.h"
#include "Client.h"
#include "MainWindow.h"

#include "GUI.h"
#include "Texture.h"

Dialogue::Dialogue(pNpcDialogue* packet, std::string npcName, int pX, int pY) {
	renderer = mainForm->renderer;
	width = 550;
	height = 120; 
	x = pX;
	y = pY;

	portraitId = packet->npcFace;
	name = "[" + npcName + "]";

	for (auto msg : packet->messages) {
		stripSquiggly(&msg);
		messages.push_back(msg);
	}

	for (auto resp : packet->responses) {
		stripSquiggly(&resp);
		responses.push_back("*" + resp);
	}

	createDialogueBox();
}

void Dialogue::stripSquiggly(std::string* str) {
	for (int i = 0; i < str->size(); i++) {
		if ((*str)[i] == '~') 
			(*str)[i] = ' ';
	}
}


void Dialogue::createDialogueBox() {
	SDL_Color npcColor = { 189, 186, 255, 255 };
	SDL_Color msgColor = { 0, 255, 255, 255 };
	SDL_Color white = { 255, 255, 255, 255 };
	int spacer = 10;
	int shiftX = x - (width / 2);

	int msgPos = 33;

	//Messages
	int msgHeight = 0;
	std::vector<Texture*> tMessages;
	for (auto msg : messages) {
		tMessages.push_back(createDialogueText(msg, MESSAGE_WRAP, msgColor));
		msgHeight += tMessages.back()->rect.h + spacer;
	}
	
	//Responses
	std::vector<Texture*> tResponses;
	int rowHeight = 0;
	for (int i = 0; i < responses.size(); i++) {
		tResponses.push_back(createDialogueText(responses[i], RESPONSE_WRAP, white));
		if (tResponses.back()->rect.h > rowHeight) rowHeight = tResponses.back()->rect.h;
		if (i % 2 || i == tResponses.size() - 1) {
			msgHeight += rowHeight + spacer;
			rowHeight = 0;
		}
	}

	height = msgPos + msgHeight;
	if (height < MINHEIGHT) height = MINHEIGHT;
	mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_SetTextureBlendMode(mainTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 45, 45, 45, 192);
	SDL_RenderClear(renderer);

	//Border
	hlineRGBA(renderer, 0, width, 0, 35, 87, 85, 128);
	hlineRGBA(renderer, 0, width, height - 1, 35, 87, 85, 192);
	vlineRGBA(renderer, 0, 0, height - 1, 35, 87, 85, 0xFF);
	vlineRGBA(renderer, width - 1, 0, height - 1, 0, 255, 255, 192);

	//portrait
	char portraitPath[32];
	sprintf(portraitPath, "data\\npcface\\icon%d.bmp", portraitId);
	portrait = new Texture(renderer);
	portrait->loadTexture(portraitPath);
	portrait->setTextureOffset(20, 40);
	SDL_RenderCopy(renderer, portrait->texture, NULL, &portrait->rect);

	//NPC Name
	Texture* tName = createDialogueText(name, 0, npcColor);
	tName->rect.x = 12; tName->rect.y = 12;
	SDL_RenderCopy(renderer, tName->texture, NULL, &tName->rect);
	delete tName;

	//Exit 
	Texture* tExit = createDialogueText("[X]", 0, white);
	exitRect = tExit->rect;
	exitRect.x = 490; exitRect.y = 12;
	SDL_RenderCopy(renderer, tExit->texture, NULL, &exitRect);
	delete tExit;

	//Messages
	msgPos = 33;
	for (auto msg : tMessages) {
		msg->rect.x = 80; msg->rect.y = msgPos;
		SDL_RenderCopy(renderer, msg->texture, NULL, &msg->rect);
		msgPos += msg->rect.h + spacer;
		delete msg;
	}

	rowHeight = 0;
	for (int i = 0; i < tResponses.size(); i++) {
		Texture* resp = tResponses[i];
		respRect[i] = resp->rect;
		respRect[i].x = 80;
		if (i % 2) respRect[i].x = 305;
		respRect[i].y = msgPos;
		SDL_RenderCopy(renderer, resp->texture, NULL, &respRect[i]);

		if (respRect[i].h > rowHeight) rowHeight = respRect[i].h;
		if (i % 2 || i == tResponses.size() - 1) {
			msgPos += rowHeight + spacer;
		}
		delete resp;
	}

	SDL_SetRenderTarget(renderer, priorTarget);

	renderRect = { x - (width / 2), y, width, height };
}

Texture* Dialogue::createDialogueText(std::string text, int maxWidth, SDL_Color color) {
	Texture *msg = nullptr;

	TTF_SetFontStyle(gui->font, TTF_STYLE_NORMAL);

	SDL_Surface* lSurface;
	if (maxWidth > 0) lSurface = TTF_RenderText_Blended_Wrapped(gui->font_npcDialogue, text.c_str(), color, maxWidth);
	else lSurface = TTF_RenderText_Blended(gui->font_npcDialogue, text.c_str(), color);

	if (lSurface) {
		SDL_Rect fontRect = { 0, 0, lSurface->w, lSurface->h };
		SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, lSurface);

		msg = new Texture(renderer, fontTexture, fontRect.w, fontRect.h);

		SDL_FreeSurface(lSurface);
	}

	return msg;
}


Dialogue::~Dialogue() {

}


void Dialogue::render() {
	SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);

	if (focusedOption >= 0) {
		SDL_Color yellow = { 255, 255, 0, 255 };
		SDL_Rect viewRect{ x - (width / 2), y, width, height };
		SDL_RenderSetViewport(renderer, &viewRect);
		
		if (focusedOption == 0) {
			//Exit
			Texture* tExit = createDialogueText("[X]", 0, yellow);
			SDL_RenderCopy(renderer, tExit->texture, NULL, &exitRect);
			delete tExit;
		}
		else {
			//Response 1+
			Texture* tResp = createDialogueText(responses[focusedOption -1], 0, yellow);
			SDL_RenderCopy(renderer, tResp->texture, NULL, &respRect[focusedOption-1]);
			delete tResp;
		}

		SDL_RenderSetViewport(renderer, NULL);
	}
}


bool Dialogue::handleEvent(SDL_Event *e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	mx -= windowOffset.x;
	my -= windowOffset.y;
	renderRect;

	SDL_Point p{ mx, my };
	if (e->type == SDL_MOUSEMOTION) {
		if (SDL_PointInRect(&p, &renderRect)) {
			p.x -= renderRect.x;
			p.y -= renderRect.y;

			focusedOption = -1;

			if (SDL_PointInRect(&p, &exitRect)) focusedOption = 0;
			else {
				for (int i = 0; i < responses.size(); i++) {
					if (SDL_PointInRect(&p, &respRect[i])) focusedOption = i + 1;
				}
			}
		}
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		if (boxClicked) {
			if (focusedOption >= 0) {
				selection = focusedOption;
			}
			boxClicked = false;
		}
	}

	if (e->type == SDL_MOUSEBUTTONDOWN) {
		boxClicked = false;
		if (doesPointIntersect(renderRect, mx, my)) {
			if (e->button.button == SDL_BUTTON_LEFT) boxClicked = true;
		}
	}

	return false;
}


void Dialogue::setPosition(int pX, int pY) {
	x = pX;
	y = pY;
	renderRect = { x - (width / 2), y - (height / 2), width, height };
}

void Dialogue::setWindowOffset(SDL_Point p) {
	windowOffset = p;
}


SDL_Rect Dialogue::getRendeRect() {
	return renderRect;
}