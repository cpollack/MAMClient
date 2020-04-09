#pragma once

#include "Widget.h"
#include "Field.h"
#include "MessageManager.h"

class CWindow;
class pMessage;
class CMessage;
class CChatField;
class CButton;

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

typedef enum ChatFilter {
	cfAll,
	cfLocal,
	cfWhisper,
	cfTeam,
	cfFriend,
	cfGuild,
	cfSystem
};

class CChat : public CWidget {
public:
	CChat(CWindow* window);
	~CChat();

	void Render();
	void HandleEvent(SDL_Event& e);
	void step();

	//void SetRenderer(SDL_Renderer* r);
	void SetFont(TTF_Font *f);
	void SetHeightInLines(int lines);
	void SetPos(SDL_Point pos);
	void SetChannel(ChatChannel channel);

	CButton* AddButton(std::string name, int btnW, int btnH, std::string btnPath, std::string btnDownPath, int toX);
	void UpdateHint();

	void AddMessage(pMessage *packet);
	void AddMessage(Message message);

private:
	TTF_Font *font;

	int x, y;
	int width, height;
	const int RowHeight = 13;
	const int MinHistHeight = 13;
	const int MinHistWidth = 310; //340 + 10 for scroll bar
	const int HeaderHeight = 25;
	const int InputHeight = 20;
	const int LineSpacer = 3;
	const SDL_Rect frameRect = { 4, 7 + 16, 310, 144 }; // Frame is offset by 16 pixels
	const SDL_Rect chatRect = { 11, 25, 298, 122 }; //14 for chat, 16 for widget rect
	const SDL_Rect inputRect = { 11, 145, 298, 20 };
	SDL_Rect mainRect, headerRect, histRect, scrollRect;
	BYTE opacity;

	Asset ChatFrame;
	bool Expanded = true;

	std::vector<CMessage*> messages;
	int rowHeight = 0;
	void render_history();

	CButton* toggledButton = nullptr;
	CButton *btnMinimize, *btnExpand;
	Asset InputField;
	CChatField* chatField = nullptr;
	SDL_Color chatColor;
	int Filter = cfAll;
	void ToggleFilter(std::string btnName, ChatFilter newFilter);

	int Channel = ccNone;
	int effect = ceNone;
	std::string Target = "All";
	std::string emotion = "";

private: //Local Reimplementation of Event Handling
	
	//typedef std::function<void(SDL_Event&)> EventFunc;
	//void registerEvent(std::string widgetName, std::string eventName, EventFunc evf);

	void chatField_Submit(SDL_Event& e);
	void btnAll_Toggle(SDL_Event& e);
	void btnLocal_Toggle(SDL_Event& e);
	void btnWhisper_Toggle(SDL_Event& e);
	void btnTeam_Toggle(SDL_Event& e);
	void btnFriend_Toggle(SDL_Event& e);
	void btnGuild_Toggle(SDL_Event& e);
	void btnSystem_Toggle(SDL_Event& e);
	void btnMinimize_Click(SDL_Event& e);
	void btnExpand_Click(SDL_Event& e);
	void btnSettings_Click(SDL_Event& e);

	bool BlockMouse = false;

public: 
	bool IsMouseOver() { return MouseOver; }
	bool IsBlockMouse() { return BlockMouse; }
};

class CMessage {
public:
	CMessage(SDL_Renderer *r, int windowWidth, pMessage *packet);
	CMessage(SDL_Renderer *r, int windowWidth, Message message);
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

	virtual void RenderText();

private:
	CChat *pChat;
};

extern CChat* chat;