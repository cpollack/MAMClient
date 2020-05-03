#pragma once

const int _DELTA_X[8] = { 0, -1, -1, -1,  0,  1, 1, 1 };
const int _DELTA_Y[8] = { 1,  1,  0, -1, -1, -1, 0, 1 };

int getDirection(SDL_Point fromCoord, SDL_Point toCoord);
SDL_Point getFrontCoord(SDL_Point coord, int direction);
SDL_Point getBackCoord(SDL_Point coord, int direction);

bool doesPointIntersect(SDL_Rect aRect, int x, int y);
bool doesPointIntersect(SDL_Rect rect, SDL_Point point);
bool doRectIntersect(SDL_Rect a, SDL_Rect b);

std::string getRoleFromLook(int look);
std::vector<std::string> getSpriteFramesFromAni(std::string role, int animation, int direction);
std::string animationTypeToString(int animation);

std::string formatInt(int value);
bool fileExist(std::string file);
void showErrorMessage(std::string message);
void applicationClose();

//Prompts
class CWindow;
class CPromptForm;
CPromptForm* doPrompt(CWindow* parent, std::string title, std::string message, bool cancel=false);
CPromptForm* doPromptError(CWindow* parent, std::string title, std::string message);

class Texture;
Texture* stringToTexture(SDL_Renderer* renderer, std::string text, TTF_Font* font, int style, SDL_Color color, int maxWidth); 
Texture* stringToTexture(SDL_Renderer* renderer, std::wstring text, TTF_Font* font, int style, SDL_Color color, int maxWidth);
std::wstring StringToWString(std::string string);