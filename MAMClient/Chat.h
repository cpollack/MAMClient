#pragma once

#include "Field.h"

class CWindow;
class pMessage;
class CMessage;
class CChatField;

enum ChatChannel {
	ccNone = 0,
	nnUnknown = 2000,
	ccPrivate = 2001,
	ccAction = 2002,
	ccTeam = 2003,
	ccGuild = 2004,
	ccSystem = 2005,
	ccSpouse = 2006,
	ccNormal = 2007,
	ccShout = 2008,
	ccFriend = 2009,
	ccBroadcast = 2010,
	ccGM = 2011,
	ccHidden = 2101,
	ccVendor = 2104,
	cWebsite = 2105
};

enum ChatEffect {
	ceNone = 0,
	ceScroll = 1,
	ceGlow = 2,
	ceScroll_Glow = 3,
	ceBlast = 8,
	ceScroll_Blast = 9,
	ceGlow_Blast = 10,
	ceScroll_Glow_Blast = 11
};

class CChat {
public:
	CChat(CWindow* window);
	~CChat();

	void render();
	void handleEvent(SDL_Event& e);

	void SetRenderer(SDL_Renderer* r);
	void SetFont(TTF_Font *f);
	void SetWidth(int w);
	void SetHeight(int h);
	void SetHeightInLines(int lines);
	void SetPos(SDL_Point pos);
	void UpdateRect();

	int GetWidth();
	int GetHeight();
	int GetX();
	int GetY();
	//SDL_Point GetPos();

	void AddMessage(pMessage *packet);

private:
	SDL_Renderer* renderer;
	TTF_Font *font;

	int x, y;
	int width, height;
	const int RowHeight = 13;
	const int MinHistHeight = 13;
	const int MinHistWidth = 310; //340 + 10 for scroll bar
	const int InputHeight = 20;
	SDL_Rect mainRect, headerRect, histRect, chatRect, scrollRect;
	BYTE opacity;

	std::vector<CMessage*> messages;
	int rowHeight = 0;
	void render_history();

	CChatField* chatField = nullptr;
	SDL_Color chatColor;

	int channel = ccNormal;
	int effect = ceNone;
	std::string target = "All";
	std::string emotion = "";

private: //Local Reimplementation of Event Handling
	std::map<std::string, CWidget*> widgets;
	typedef std::function<void(SDL_Event&)> EventFunc;
	void registerEvent(std::string widgetName, std::string eventName, EventFunc evf);

	void chatField_Submit(SDL_Event& e);
};

class CMessage {
public:
	CMessage(SDL_Renderer *r, int windowWidth, pMessage *packet);
	~CMessage();

	void render(int maxWidth, int bottomY);
	void renderText(std::string renderString, SDL_Color withColor, SDL_Point toPoint, int *advance);

	void SetWidth(int w);
	void SetFont(TTF_Font *f);
	int GetRows();
	int GetRows(int maxW);

	SDL_Renderer *renderer;
	TTF_Font *font;

	std::string sender;
	std::string target;
	std::string message;
	
	std::string fullMessage;
	std::string strChannel;

	SDL_Color color;
	std::string emotion;
	ChatChannel channel;
	ChatEffect effect;

	const int LineSpacer = 3;
	int maxWidth;
	int rows = 0;
	int textHeight;

private:
	void SetFullMessage();
	void SetRows();
};

class CChatField : public CField {
public:
	CChatField(CWindow* window, CChat *chat, std::string name, int x, int y);
	virtual void Render();
	virtual void HandleEvent(SDL_Event& e);

private:
	CChat *pChat;
};

extern CChat* chat;