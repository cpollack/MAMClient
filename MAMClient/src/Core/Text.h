#pragma once

class Text {
public:
	Text(SDL_Renderer* r, std::string sLine, int toX, int toY, bool isBold = false);
	~Text();

	SDL_Renderer *renderer;
	std::string text;
	SDL_Texture *texture;
	SDL_Rect rect;

	int alignment;
	TTF_Font *font;
	SDL_Color color;
	bool bold, underlined;
	int wrapLength = 0;

	void setPosition(int px, int py);
	void setX(int x);
	void setY(int y);

	void setText(std::string sText);
	void RenderText();

	void render();
	//void offsetPosition(SDL_Point p);
	//void setAlignment(int algn);
	int getWidth();
	int getHeight();

private:
	SDL_Rect getTextRect();
};

using PText = std::shared_ptr<Text>;