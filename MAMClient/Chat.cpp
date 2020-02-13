#include "stdafx.h"
#include "Chat.h"

#include "Player.h"
#include "MessageManager.h"
#include "pMessage.h"

#include "CustomEvents.h"
#include "Window.h"
#include "Widget.h"
#include "Field.h"
#include "Button.h"

CChat::CChat(CWindow* window) {
	this->window = window;
	renderer = window->renderer;
	opacity = 128;

	chatField = new CChatField(window, this, "chatField", 0, 0);
	chatField->SetHeight(InputHeight);
	widgets["chatField"] = chatField;
	registerEvent("chatField", "Submit", std::bind(&CChat::chatField_Submit, this, std::placeholders::_1));

	AddButton("btnAll", 40, 25, "GUI/chat/All.png", "GUI/chat/AllDown.png", 0)->SetType(btToggle);
	AddButton("btnLocal", 40, 25, "GUI/chat/Local.png", "GUI/chat/LocalDown.png", 40)->SetType(btToggle);
	AddButton("btnWhisper", 40, 25, "GUI/chat/Whisper.png", "GUI/chat/WhisperDown.png", 80)->SetType(btToggle);
	AddButton("btnTeam", 40, 25, "GUI/chat/Team.png", "GUI/chat/TeamDown.png", 120)->SetType(btToggle);
	AddButton("btnFriend", 40, 25, "GUI/chat/Friend.png", "GUI/chat/FriendDown.png", 160)->SetType(btToggle);
	AddButton("btnGuild", 40, 25, "GUI/chat/Guild.png", "GUI/chat/GuildDown.png", 200)->SetType(btToggle);
	AddButton("btnSystem", 40, 25, "GUI/chat/System.png", "GUI/chat/SystemDown.png", 240)->SetType(btToggle);
	AddButton("btnSettings", 20, 20, "GUI/chat/Cog.png", "GUI/chat/CogDown.png", 0);

	registerEvent("btnAll", "ToggleOn", std::bind(&CChat::btnAll_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnLocal", "ToggleOn", std::bind(&CChat::btnLocal_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnWhisper", "ToggleOn", std::bind(&CChat::btnWhisper_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnTeam", "ToggleOn", std::bind(&CChat::btnTeam_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnFriend", "ToggleOn", std::bind(&CChat::btnFriend_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnGuild", "ToggleOn", std::bind(&CChat::btnGuild_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnSystem", "ToggleOn", std::bind(&CChat::btnSystem_ToggleOn, this, std::placeholders::_1));
	registerEvent("btnSettings", "Click", std::bind(&CChat::btnSettings_Click, this, std::placeholders::_1));

	UpdateRect();
	chatColor = { 255, 255, 255, opacity };
	((CButton*)widgets["btnAll"])->Toggle(true);
	SetChannel(ccNormal);
}

CChat::~CChat() {

}

void CChat::render() {
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);

	SDL_Texture *mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetTextureBlendMode(mainTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0x00);
	SDL_RenderClear(renderer);

	//Draw the main texture first
	int x2 = mainRect.x + mainRect.w;
	int y2 = mainRect.y + mainRect.h;
	roundedBoxRGBA(renderer, 0, 0, mainRect.w, mainRect.h, 10, 0, 0, 0, opacity);

	//draw header with tabs

	//draw chat history here
	//rectangleRGBA(renderer, histRect.x, histRect.y, histRect.x + histRect.w, histRect.y + histRect.h, 255, 255, 255, opacity);

	SDL_Texture *histTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, histRect.w, histRect.h);
	SDL_SetTextureBlendMode(histTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, histTexture);
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0x00);
	SDL_RenderClear(renderer);

	render_history();

	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_RenderCopy(renderer, histTexture, NULL, &histRect);

	//draw scrollbar onto history when applicable

	//draw chat input here
	rectangleRGBA(renderer, chatRect.x, chatRect.y, chatRect.x + chatRect.w, chatRect.y + chatRect.h, 255, 255, 255, opacity);

	for (auto widget : widgets) widget.second->Render();
	//chatField->Render();

	SDL_DestroyTexture(histTexture); //this breaks rectangleRGBA for some reason

	SDL_SetRenderTarget(renderer, priorTarget);
	SDL_RenderCopy(renderer, mainTexture, NULL, &mainRect);
	SDL_DestroyTexture(mainTexture);
}

void CChat::render_history() {
	int heightUsed = 0;
	for (int i = messages.size(); i > 0; i--) {
		CMessage *curMessage = messages[i - 1];

		bool show = false;
		switch (curMessage->channel) {
		case ccNormal:
			if (Filter == cfAll || Filter == cfLocal) show = true;
			break;
		case ccPrivate:
			show = true;
			if (Filter == cfSystem) show = false;
			break;
		case ccTeam:
			if (Filter == cfAll || Filter == cfTeam) show = true;
			break;
		case ccFriend:
			if (Filter == cfAll || Filter == cfFriend) show = true;
			break;
		case ccGuild:
			if (Filter == cfAll || Filter == cfGuild) show = true;
			break;
		case ccSpouse:
			show = false;
			if (Filter == cfAll || Filter == cfWhisper || Filter == cfFriend) show = true;
			break;

		case ccSystem:
			show = true;
		case ccHidden:
			break;

		default:
			show = true;
		}
		if (!show) continue;

		curMessage->render(width, histRect.h - heightUsed);
		heightUsed += curMessage->textHeight;
		
		if (heightUsed >= histRect.h) break;
	}
}

void CChat::handleEvent(SDL_Event& e) {
	if (chatField->IsFocus()) {
		if (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT) {
			chatField->HandleEvent(e);
			return;
		}
	}
	else {
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_RETURN2) window->FocusWidget(chatField);
		}
	}

	if (e.type == SDL_MOUSEMOTION) {
		MouseOver = false;
		if (doesPointIntersect(SDL_Rect{ x,y,width,height }, SDL_Point{ e.motion.x, e.motion.y })) {
			MouseOver = true;
		}
	}

	if (!MouseOver) {
		if (e.type == SDL_MOUSEBUTTONDOWN) chatField->LoseFocus();
		return;
	}
	
	SDL_Event e2 = e;
	if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
		e2.motion.x -= x;
		e2.motion.y -= y;
		BlockMouse = false;
	}
	for (auto widget : widgets) {
		widget.second->HandleEvent(e2);
		if (widget.second->IsMouseOver()) BlockMouse = true;
	}	

	if (!BlockMouse) {
		if (e.type == SDL_MOUSEBUTTONDOWN) chatField->LoseFocus();
	}
}

void CChat::step() {
	Message message = messageManager.Poll();
	if (message.get()) AddMessage(message);
}

void CChat::registerEvent(std::string widgetName, std::string eventName, EventFunc evf) {
	auto iter = widgets.find(widgetName);
	if (iter != widgets.end()) {
		iter->second->RegisterEvent(eventName, evf);
	}
}

/* Event hooks go here */

CButton* CChat::AddButton(std::string name, int btnW, int btnH, std::string btnPath, std::string btnDownPath, int toX) {
	CButton* newBtn = new CButton(window, name, toX, 0);
	newBtn->SetWidth(btnW);
	newBtn->SetHeight(btnH);
	newBtn->SetUnPressedImage(btnPath);
	newBtn->SetPressedImage(btnDownPath);
	widgets[name] = newBtn;
	return newBtn;
}

void CChat::UpdateRect() {
	mainRect = { x, y, width, height };
	headerRect = { 0, height - HeaderHeight, width, HeaderHeight };
	chatRect = { 0, height - HeaderHeight - InputHeight, width, InputHeight };
	histRect = { 0, 0, width, height - headerRect.h - chatRect.h };
	scrollRect = { width - 8, histRect.y, 8, histRect.h };

	chatField->SetX(chatRect.x + 5);
	chatField->SetY(chatRect.y);
	chatField->SetWidth(chatRect.w - 10);

	widgets["btnAll"]->SetY(headerRect.y);
	widgets["btnLocal"]->SetY(headerRect.y);
	widgets["btnWhisper"]->SetY(headerRect.y);
	widgets["btnTeam"]->SetY(headerRect.y);
	widgets["btnFriend"]->SetY(headerRect.y);
	widgets["btnGuild"]->SetY(headerRect.y);
	widgets["btnSystem"]->SetY(headerRect.y);

	widgets["btnSettings"]->SetX(headerRect.w - widgets["btnSettings"]->GetWidth());
	widgets["btnSettings"]->SetY(headerRect.y + HeaderHeight - widgets["btnSettings"]->GetHeight());
}

void CChat::UpdateHint() {
	std::string hint = "Talking in ";
	switch (Channel) {
	case ccNormal:
		hint += "[Local]";
		break;
	case ccPrivate:
		hint += "[Whisper]";
		break;
	case ccTeam:
		hint += "[Team]";
		break;
	case ccFriend:
		hint += "[Friend]";
		break;
	case ccGuild:
		hint += "[Guild]";
		break;
	}
	hint += " @" + Target;
	chatField->SetHint(hint);
}

/*void CChat::SetRenderer(SDL_Renderer* r) {
	renderer = r;
	//chatField->SetRenderer(renderer);
	UpdateRect();
	//chatField->SetText("Test Text!");
	//chatField->SetHint("Talking in [Normal]");
}*/

void CChat::SetFont(TTF_Font *f) {
	font = f;
}

void CChat::SetWidth(int w) {
	width = w;
	UpdateRect();
}

void CChat::SetHeight(int h) {
	height = h;
	UpdateRect();
}

void CChat::SetHeightInLines(int lines) {
	SetHeight(((TTF_FontHeight(font) + LineSpacer) * lines) + HeaderHeight + InputHeight);
}

void CChat::SetPos(SDL_Point pos) {
	x = pos.x;
	y = pos.y;
	UpdateRect();
}

void CChat::SetChannel(ChatChannel channel) {
	if (Channel == channel) return;
	Channel = channel;
	UpdateHint();
}

void CChat::AddMessage(pMessage *packet) {
	CMessage *newMessage = new CMessage(renderer, width, packet);
	newMessage->SetFont(font);
	newMessage->GetRows();
	rowHeight += newMessage->GetRows();
	messages.push_back(newMessage);
}

void CChat::AddMessage(Message message) {
	CMessage *newMessage = new CMessage(renderer, width, message);
	newMessage->SetFont(font);
	newMessage->GetRows();
	rowHeight += newMessage->GetRows();
	messages.push_back(newMessage);
}

int CChat::GetWidth() {
	return width;
}

int CChat::GetHeight() {
	return height;
}

int CChat::GetX() {
	return x;
}

int CChat::GetY() {
	return y;
}

void CChat::ToggleFilter(std::string btnName, ChatFilter newFilter) {
	CButton* newBtn = (CButton*)widgets.find(btnName)->second;
	if (!newBtn || toggledButton == newBtn) return;

	if (toggledButton) toggledButton->Toggle(false);
	toggledButton = newBtn;
	Filter = newFilter;
}

void CChat::chatField_Submit(SDL_Event& e) {
	if (CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WIDGET;
		event.window.windowID = e.window.windowID;
		event.user.code = WIDGET_FOCUS_LOST;
		event.user.data1 = chatField;
		event.user.data2 = window;
		SDL_PushEvent(&event);
	}

	std::string chatText = chatField->GetText();
	if (chatText.length() == 0) return;

	//Check if this is a valid command
	if (chatText[0] == '/') {
		std::locale loc;
		std::string str = chatText;
		for (int i = 0; i < str.size(); i++) str[i] = std::toupper(str[i], loc);

		if (str.compare("/L") == 0 || str.compare("/LOCAL") == 0) {
			SetChannel(ccNormal);
		}
		if (str.compare("/W") == 0 || str.compare("/WHISPER") == 0) {
			SetChannel(ccPrivate);
		}
		if (str.compare("/T") == 0 || str.compare("/TEAM") == 0) {
			SetChannel(ccTeam);
		}
		if (str.compare("/F") == 0 || str.compare("/FRIEND") == 0) {
			SetChannel(ccFriend);
		}
		if (str.compare("/G") == 0 || str.compare("/GUILD") == 0) {
			SetChannel(ccGuild);
		}
		chatField->SetText("");
		return;
	}

	if (chatText[0] == '@') {
		std::string str = chatText.substr(1);
		if (str.size() <= 16 && str.size() >= 1) {
			Target = str;
			UpdateHint();
		}
		chatField->SetText("");
		return;
	}

	int iColor = ((chatColor.r & 0xFF) << 16) | ((chatColor.g & 0xFF) << 8) | (chatColor.b & 0xFF);
	pMessage *sendMessage = new pMessage(Channel, player->GetName(), Target, chatText, iColor, effect, emotion);
	AddMessage(sendMessage);
	gClient.addPacket(sendMessage);
	chatField->SetText("");
}

void CChat::btnAll_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnAll", cfAll);
}

void CChat::btnLocal_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnLocal", cfLocal);
	SetChannel(ccNormal);
}

void CChat::btnWhisper_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnWhisper", cfWhisper);
	SetChannel(ccPrivate);
}

void CChat::btnTeam_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnTeam", cfTeam);
	SetChannel(ccTeam);
}

void CChat::btnFriend_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnFriend", cfFriend);
	SetChannel(ccFriend);
}

void CChat::btnGuild_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnGuild", cfGuild);
	SetChannel(ccGuild);
}

void CChat::btnSystem_ToggleOn(SDL_Event& e) {
	ToggleFilter("btnSystem", cfSystem);
}

void CChat::btnSettings_Click(SDL_Event& e) {
	//
}

/* - CMessage - */

CMessage::CMessage(SDL_Renderer *r, int windowWidth, pMessage *packet) {
	renderer = r;
	maxWidth = windowWidth;

	sender = packet->sender;
	target = packet->target;
	message = packet->message;

	channel = (ChatChannel)packet->channel;
	effect = (ChatEffect)packet->effect;
	emotion = packet->emotion;
	color = { (Uint8)((packet->color & 0xFF0000) >> 16), (Uint8)((packet->color & 0xFF00) >> 8), (Uint8)(packet->color & 0xFF), (Uint8)0x255 };

	SetFullMessage();
}

CMessage::CMessage(SDL_Renderer *r, int windowWidth, Message message) {
	renderer = r;
	maxWidth = windowWidth;

	sender = message->sender;
	target = message->target;
	this->message = message->message;

	channel = (ChatChannel)message->channel;
	effect = (ChatEffect)message->effect;
	emotion = message->emotion;
	color = message->color;

	SetFullMessage();
}

CMessage::~CMessage() {
	//
}

void CMessage::render(int maxWidth, int bottomY) {
	this->maxWidth = maxWidth;
	//Color for Source/Target 173,255,173
	//color for 'speaks to' 0xFFFFFF
	//color for general: 0,255,0
	//color for System is always 255,0,0
	//emotion to 0,255,255

	textHeight = 0;
	int singleRowHeight = TTF_FontHeight(font) + LineSpacer;
	int rowHeight = GetRows(maxWidth) * singleRowHeight;
	int lastChar = 0;

	int advance;
	TTF_GlyphMetrics(font, ' ', NULL, NULL, NULL, NULL, &advance);
	int spaceAdvance = advance;

	int row = 1;
	int rowPos;

	while (rowHeight > 0) {
		int curRowWidth = 0;
		int curRowHeight = bottomY - rowHeight;
		rowPos = 0;

		if (row == 1) {
			//Channel
			SDL_Color channelColor;
			if (channel == ccSystem) {
				channelColor = { 255, 0, 255, 255 }; //purple
			}
			else {
				//check for 'related to me'
				//Color for from/to me: 255,0,255 //purple

				channelColor = { 0, 255, 0, 255 }; //green
			}
			renderText(strChannel, channelColor, SDL_Point{ rowPos, curRowHeight }, &advance);
			rowPos += advance + spaceAdvance;

			if (channel != ccSystem) {
				//Sender
				SDL_Color stColor = { 173, 255, 173, 255 };
				renderText(sender, stColor, SDL_Point{ rowPos, curRowHeight }, &advance);
				rowPos += advance + spaceAdvance;

				//speaks to
				SDL_Color whiteColor = { 255, 255, 255, 255 };
				renderText("speaks to", whiteColor, SDL_Point{ rowPos, curRowHeight }, &advance);
				rowPos += advance + spaceAdvance;

				//Target
				renderText(target, stColor, SDL_Point{ rowPos, curRowHeight }, &advance);
				rowPos += advance + spaceAdvance;
			}
		}

		std::string outputLine = "";
		curRowWidth += rowPos;
		while (lastChar < message.size()) {
			TTF_GlyphMetrics(gui->chatFont, message[lastChar], NULL, NULL, NULL, NULL, &advance);
			if (curRowWidth + advance < maxWidth) {
				outputLine.push_back(message[lastChar++]);
				curRowWidth += advance;
			}
			else break;
		}

		//Message
		renderText(outputLine, color, SDL_Point{ rowPos, curRowHeight }, &advance);

		textHeight += singleRowHeight;
		rowHeight -= singleRowHeight;
		row++;
	}
}

void CMessage::renderText(std::string renderString, SDL_Color withColor, SDL_Point toPoint, int *advance) {
	//Luma of font color dictates a dark or bright shadow
	SDL_Color shadow;
	int luma = (withColor.r * 2 + withColor.b + withColor.g * 3) / 6;
	if (luma > 50) shadow = { 32, 32, 32, 192 };
	else shadow = { 192, 192, 192, 192 };

	//SDL_Surface *textSurface = TTF_RenderText_Blended(gui->chatFont, renderString.c_str(), withColor);
	SDL_Surface *textSurface = TTF_RenderUNICODE_Solid(gui->fontUni, (const Uint16*)StringToWString(renderString).c_str(), withColor);
	//SDL_Surface *shadowSurface = TTF_RenderText_Blended(gui->chatFont, renderString.c_str(), shadow);
	SDL_Surface *shadowSurface = TTF_RenderUNICODE_Solid(gui->fontUni, (const Uint16*)StringToWString(renderString).c_str(), shadow);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);

	int w = textSurface->w;
	int h = textSurface->h;
	SDL_FreeSurface(textSurface);
	SDL_FreeSurface(shadowSurface);

	*advance = 0;
	if (textTexture) {
		int toWidth = w > maxWidth ? maxWidth : w;

		SDL_Rect srcRect = { 0, 0, toWidth, h };
		SDL_Rect destRect = { toPoint.x, toPoint.y, toWidth, h };

		SDL_SetTextureBlendMode(textTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(shadowTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

		SDL_RenderCopy(renderer, shadowTexture, &srcRect, &destRect);
		
		destRect.x -= 1;
		destRect.y -= 1;
		SDL_RenderCopy(renderer, textTexture, &srcRect, &destRect);
		//SDL_RenderCopy(renderer, textTexture, &srcRect, &destRect);

		*advance = w;
		SDL_DestroyTexture(textTexture);
		SDL_DestroyTexture(shadowTexture);
	}
}

void CMessage::SetFont(TTF_Font *f) {
	font = f;
}

void CMessage::SetFullMessage() {
	strChannel = "[";
	switch (channel) {
	case ccNone:
		break;
	case nnUnknown:
		break;
	case ccPrivate:
		strChannel += "Whisper";
		break;
	case ccAction:
		strChannel += "Action";
		break;
	case ccTeam:
		strChannel += "Team";
		break;
	case ccGuild:
		strChannel += "Guild";
		break;
	case ccSystem:
		strChannel += "System";
		break;
	case ccSpouse:
		strChannel += "Spouse";
		break;
	case ccNormal:
		strChannel += "Normal";
		break;
	case ccShout:
		strChannel += "Shout";
		break;
	case ccFriend:
		strChannel += "Friend";
		break;
	case ccBroadcast:
		strChannel += "Broadcast";
		break;
	case ccGM:
		strChannel += "GM";
		break;
	case ccVendor:
		strChannel += "Vendor";
		break;
	}
	strChannel += "]";
	fullMessage = strChannel;

	if (channel != ccSystem) {
		if (sender.length() > 0) {
			fullMessage += sender + " ";
			if (emotion.length() > 0) fullMessage += emotion + " ";
			fullMessage += "speaks to ";
		}

		if (target.length() > 0) fullMessage += target + " ";
	}
	else fullMessage += " ";

	if (message.length() > 0) fullMessage += message;
}

void CMessage::SetWidth(int w) {
	maxWidth = w;
}

void CMessage::SetRows() {
	rows = 1;
	int advance = 0;
	int curWidth = 0;

	for (int i = 0; i < fullMessage.length(); i++) {
		TTF_GlyphMetrics(gui->chatFont, fullMessage[i], NULL, NULL, NULL, NULL, &advance);
		curWidth += advance;
		if (curWidth > maxWidth) {
			rows++;
			curWidth = advance;
		}
	}
}

int CMessage::GetRows(int maxW) {
	SetWidth(maxW);
	SetRows();
	return rows;
}

int CMessage::GetRows() {
	SetRows();
	return rows;
}

CChatField::CChatField(CWindow* window, CChat *chat, std::string name, int x, int y) : CField (window, name, x, y) {
	pChat = chat;
}

void CChatField::Render() {
	if (!Visible) return;
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);

	//Render the background UI for the widget
	//no bg for chat field?

	//Render the actual text
	SDL_Rect textRect{ X + 5, Y + ((Height / 2) - (fontRect.h / 2)), fontRect.w, fontRect.h };
	if (Text.length() == 0) {
		if (hintTexture) {
			SDL_Rect hintRect2 = { X + 5, Y + ((Height / 2) - (hintRect.h / 2)), hintRect.w, hintRect.h };
			SDL_RenderCopy(renderer, hintTexture, NULL, &hintRect2);
		}
	}
	else {
		SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);
	}

	//Render cursor
	if (Focused) {
		cursorFrame++;
		int nextFrame = cursorFrame % 60;
		if (nextFrame < 30) {
			int cursorX = 0;
			if (cursorPos == Text.size()) {
				cursorX = textRect.x + textRect.w;
			}

			if (cursorPos == 0) {
				cursorX = textRect.x;
			}

			int ascent = TTF_FontAscent(gui->chatFont);
			int cursorY1 = textRect.y - (textRect.h ? 0 : ascent / 2 );
			int cursorY2 = cursorY1 + ascent;
			vlineRGBA(renderer, cursorX, cursorY1, cursorY2, 0xE3, 0xE3, 0xE3, 0xFF);
		}
	}

	SDL_SetRenderTarget(renderer, priorTarget);
}

void CChatField::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e);

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (MouseOver) {
			held = true;
			OnClick(e);
			//OnFocus();
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		held = false;
	}

	if (Focused && e.type == SDL_KEYDOWN)
	{
		OnKeyDown(e);
	}

	if (Focused && e.type == SDL_TEXTINPUT)
	{
		//Not copy or pasting
		/*if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
		{
		//Append character
		inputText += e.text.text;
		renderText = true;
		}*/

		OnTextInput(e);
		return;
	}
}
