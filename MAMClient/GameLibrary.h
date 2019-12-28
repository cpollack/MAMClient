#pragma once

const int _DELTA_X[8] = { 0, -1, -1, -1,  0,  1, 1, 1 };
const int _DELTA_Y[8] = { 1,  1,  0, -1, -1, -1, 0, 1 };

int getDirection(SDL_Point fromCoord, SDL_Point toCoord);
bool doesPointIntersect(SDL_Rect aRect, int x, int y);
bool doesPointIntersect(SDL_Rect rect, SDL_Point point);

std::string getRoleFromLook(int look);
std::vector<std::string> getSpriteFramesFromAni(std::string role, int animation, int direction);
std::string animationTypeToString(int animation);

std::string formatInt(int value);
bool fileExist(std::string file);
void showErrorMessage(std::string message);
void applicationClose();

//Prompts
class CPromptForm;
CPromptForm* doPrompt(std::string title, std::string message); 
CPromptForm* doPromptError(std::string title, std::string message);

class Texture;
Texture* stringToTexture(SDL_Renderer* renderer, std::string text, TTF_Font* font, int style, SDL_Color color, int maxWidth);