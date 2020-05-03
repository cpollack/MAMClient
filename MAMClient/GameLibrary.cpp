#include "stdafx.h"
#include "GameLibrary.h"
#include "CustomEvents.h"
#include "Global.h"

#include "INI.h"
#include "Sprite.h"

#include "PromptForm.h"

int getDirection(SDL_Point fromPos, SDL_Point toPos) {
	static const double TWOPI = 6.2831853071795865;
	static const double RAD2DEG = 57.2957795130823209;
	static const double ANGLEDIR = 45;

	double theta = atan2(fromPos.x - toPos.x, fromPos.y - toPos.y);
	if (theta < 0.0)
		theta += TWOPI;
	double bearing = RAD2DEG * theta;
	bearing += 180;
	if (bearing > 360) bearing -= 360;
	bearing -= (ANGLEDIR / 2);
	if (bearing < 0) bearing += 360;
	bearing = 360 - bearing;

	int newDirection = (int)std::round(bearing) / 45;
	newDirection -= 1;
	if (newDirection < 0) newDirection += 8;
	return newDirection;
}

SDL_Point getFrontCoord(SDL_Point coord, int direction) {
	SDL_Point frontCoord = coord;
	frontCoord.x += _DELTA_X[direction];
	frontCoord.y += _DELTA_Y[direction];
	return frontCoord;
}

SDL_Point getBackCoord(SDL_Point coord, int direction) {
	SDL_Point backCoord = coord;
	backCoord.x += _DELTA_X[direction] * -1;
	backCoord.y += _DELTA_Y[direction] * -1;
	return backCoord;
}


bool doesPointIntersect(SDL_Rect rect, int x, int y) {
	if (x >= rect.x && x <= (rect.x + rect.w) && y >= rect.y && y <= (rect.y + rect.h)) return true;
	return false;
}


bool doesPointIntersect(SDL_Rect rect, SDL_Point point) {
	if (point.x >= rect.x && point.x <= (rect.x + rect.w) && point.y >= rect.y && point.y <= (rect.y + rect.h)) return true;
	return false;
}

bool doRectIntersect(SDL_Rect a, SDL_Rect b) {
	if (a.x <= b.x + b.w - 1 && a.x + a.w - 1 >= b.x &&
		a.y <= b.y + b.h - 1 && a.y + a.h - 1 >= b.y) return true;
	return false;
}


std::string getRoleFromLook(int look) {
	INI roleINI("INI/Roles.ini", "RolesInfo");
	std::string entry = "Role" + std::to_string(look);
	std::string role = roleINI.getEntry(entry);
	return role;
}


std::vector<std::string> getSpriteFramesFromAni(std::string role, int animation, int direction) {
	std::string aniFile = "ANI\\" + role + ".ani";
	std::string aniSection = animationTypeToString(animation) + std::to_string(direction);
	INI ani(aniFile, aniSection);

	std::vector<std::string> frames;
	for (int i = 0; i < stoi(ani.getEntry("FrameAmount")); i++) {
		std::string nextFrame = "Frame" + std::to_string(i);
		frames.push_back(ani.getEntry(nextFrame));
	}

	return frames;
}


std::string animationTypeToString(int animation) {
	switch (animation) {
	case Attack01: return "Attack01";
	case Attack02: return "Attack02";
	case Attack03: return "Attack03";
	case Cast: return "Cast";
	case Defend: return "defend";
	case Faint: return "Faint";
	case Fill: return "Fill";
	case Fury: return "Fury";
	case Genuflect: return "Genuflect";
	case Laugh: return "Laugh";
	case Lie: return "Lie";
	case Politeness: return "Politeness";
	case Sad: return "Sad";
	case SayHello: return "SayHello";
	case SitDown: return "SitDown";
	case Specialties_Attack: return "Specialties_attack";
	case StandBy: return "StandBy";
	case Walk: return "Walk";
	case Wound: return "Wound";
	}
}

std::string formatInt(int value) {
	std::string numWithCommas = std::to_string(value);
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	return numWithCommas;
}

bool fileExist(std::string file) {
	if (!file.length()) return false;
	struct stat buffer;
	if (stat(file.c_str(), &buffer) != 0) return false;
	return true;
}

void showErrorMessage(std::string message) {
	MessageBox(0, message.c_str(), "Error", MB_OK);
}

void applicationClose() {
	bExitApplication = true;
	SDL_Event quitEvent;
	quitEvent.type = SDL_QUIT;
	SDL_PushEvent(&quitEvent);
}

CPromptForm* doPrompt(CWindow* parent, std::string title, std::string message, bool cancel) {
	CPromptForm* promptForm = new CPromptForm(cancel);
	promptForm->SetParent(parent);
	promptForm->SetTitle(title);
	promptForm->SetMessage(message);
	Windows.push_back(promptForm);
	return promptForm;
}

CPromptForm* doPromptError(CWindow* parent, std::string title, std::string message) {
	CPromptForm* promptForm = doPrompt(parent, title, message);
	promptForm->SetType(WINDOW_CLOSE_PROMPT_ERROR);
	return promptForm;
}

Texture* stringToTexture(SDL_Renderer* renderer, std::string text, TTF_Font* font, int style = 0, SDL_Color color = {0 ,0, 0, 255}, int maxWidth = 0) {
	Texture *texture = nullptr;

	TTF_SetFontStyle(gui->font, style);

	SDL_Surface* lSurface;
	if (maxWidth > 0) lSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, maxWidth);
	else lSurface = TTF_RenderText_Blended(gui->font_npcDialogue, text.c_str(), color);

	if (lSurface) {
		SDL_Rect fontRect = { 0, 0, lSurface->w, lSurface->h };
		SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, lSurface);

		texture = new Texture(renderer, fontTexture, fontRect.w, fontRect.h);

		SDL_FreeSurface(lSurface);
	}

	TTF_SetFontStyle(gui->font, TTF_STYLE_NORMAL);

	return texture;
}

Texture* stringToTexture(SDL_Renderer* renderer, std::wstring text, TTF_Font* font, int style = 0, SDL_Color color = { 0 ,0, 0, 255 }, int maxWidth = 0) {
	Texture *texture = nullptr;

	TTF_SetFontStyle(gui->font, style);

	SDL_Surface* lSurface;
	if (maxWidth > 0) lSurface = TTF_RenderUNICODE_Blended_Wrapped(font, (const Uint16*)text.c_str(), color, maxWidth);
	else lSurface = TTF_RenderUNICODE_Blended(gui->font_npcDialogue, (const Uint16*)text.c_str(), color);

	if (lSurface) {
		SDL_Rect fontRect = { 0, 0, lSurface->w, lSurface->h };
		SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, lSurface);

		texture = new Texture(renderer, fontTexture, fontRect.w, fontRect.h);

		SDL_FreeSurface(lSurface);
	}

	TTF_SetFontStyle(gui->font, TTF_STYLE_NORMAL);

	return texture;
}

std::wstring StringToWString(std::string string) {
	wchar_t *wc = new wchar_t[string.length() + 1];
	mbstowcs(wc, string.c_str(), string.length() + 1);
	std::wstring wstring(wc);
	delete[] wc;
	return wstring;
}