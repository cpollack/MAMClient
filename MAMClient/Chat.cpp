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

CChat::CChat(CWindow* window) : CWidget(window) {
	opacity = 0;
	CWidget::SetWidth(320);
	CWidget::SetHeight(176);
	SetX(0);
	SetY(window->GetHeight() - Height);

	ChatFrame.reset(new Texture(renderer, "data/GUI/Chat/chat.png"));
	ChatFrame->setBlendMode(SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(ChatFrame->texture, 192);
	InputField.reset(new Texture(renderer, "data/GUI/Chat/input.png"));
	chatField = new CChatField(window, this, "chatField", inputRect.x + 2, inputRect.y);
	chatField->SetWidth(inputRect.w - 4);
	chatField->SetHeight(inputRect.h);
	widgets["chatField"] = chatField;
	RegisterEvent("chatField", "Submit", std::bind(&CChat::chatField_Submit, this, std::placeholders::_1));

	btnMinimize = new CButton(window, "btnMinimize", 285, 11);
	btnMinimize->SetWidth(23);
	btnMinimize->SetHeight(13);
	btnMinimize->SetUnPressedImage("data/GUI/chat/btnMinimize.png");
	btnMinimize->SetPressedImage("data/GUI/chat/btnMinimize.png");
	widgets["btnMinimize"] = btnMinimize;
	btnExpand = new CButton(window, "btnExpand", 0, window->GetHeight() - 30);
	btnExpand->SetWidth(153);
	btnExpand->SetHeight(30);
	btnExpand->SetUnPressedImage("data/GUI/chat/btnExpand.png");
	btnExpand->SetPressedImage("data/GUI/chat/btnExpand.png");
	btnExpand->SetVisible(false);
	widgets["btnExpand"] = btnExpand;

	AddButton("btnAll", 50, 30, "data/GUI/chat/tabAll.png", "data/GUI/chat/tabAll2.png", 0)->SetType(btToggle);
	AddButton("btnLocal", 50, 30, "data/GUI/chat/tabLocal.png", "data/GUI/chat/tabLocal2.png", 40)->SetType(btToggle);
	AddButton("btnWhisper", 50, 30, "data/GUI/chat/tabWhisper.png", "data/GUI/chat/tabWhisper2.png", 80)->SetType(btToggle);
	AddButton("btnTeam", 50, 30, "data/GUI/chat/tabTeam.png", "data/GUI/chat/tabTeam2.png", 120)->SetType(btToggle);
	AddButton("btnFriend", 50, 30, "data/GUI/chat/tabFriend.png", "data/GUI/chat/tabFriend2.png", 160)->SetType(btToggle);
	AddButton("btnGuild", 50, 30, "data/GUI/chat/tabGuild.png", "data/GUI/chat/tabGuild2.png", 200)->SetType(btToggle);
	AddButton("btnSystem", 50, 30, "data/GUI/chat/tabSystem.png", "data/GUI/chat/tabSystem2.png", 240)->SetType(btToggle);
	//AddButton("btnSettings", 20, 20, "data/GUI/chat/Cog.png", "data/GUI/chat/CogDown.png", 0);

	RegisterEvent("btnAll", "Toggle", std::bind(&CChat::btnAll_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnLocal", "Toggle", std::bind(&CChat::btnLocal_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnWhisper", "Toggle", std::bind(&CChat::btnWhisper_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnTeam", "Toggle", std::bind(&CChat::btnTeam_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnFriend", "Toggle", std::bind(&CChat::btnFriend_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnGuild", "Toggle", std::bind(&CChat::btnGuild_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnSystem", "Toggle", std::bind(&CChat::btnSystem_Toggle, this, std::placeholders::_1));
	RegisterEvent("btnMinimize", "Click", std::bind(&CChat::btnMinimize_Click, this, std::placeholders::_1));
	RegisterEvent("btnExpand", "Click", std::bind(&CChat::btnExpand_Click, this, std::placeholders::_1));
	//registerEvent("btnSettings", "Click", std::bind(&CChat::btnSettings_Click, this, std::placeholders::_1));

	chatColor = { 255, 255, 255, opacity };
	((CButton*)widgets["btnAll"])->Load();
	((CButton*)widgets["btnAll"])->Toggle(true);
	SetChannel(ccNormal);
}

CChat::~CChat() {

}

void CChat::Render() {
	SDL_Rect viewPort;
	SDL_RenderGetViewport(renderer, &viewPort);
	if (Expanded) {
		SDL_RenderSetViewport(renderer, &widgetRect);

		for (auto widget : widgets) {
			if (widget.second->Name != "chatField" && 
				widget.second->Name != "btnExpand") widget.second->Render();
		}
		SDL_Rect destRect = { 0, 16, ChatFrame->width, ChatFrame->height };
		SDL_RenderCopy(renderer, ChatFrame->texture, NULL, &destRect);
		if (chatField->IsFocus()) {
			destRect = { 7, 143, InputField->width, InputField->height };
			SDL_RenderCopy(renderer, InputField->texture, NULL, &destRect);
		}
		widgets["chatField"]->Render();

		SDL_Rect histViewport = chatRect;
		histViewport.x += widgetRect.x;
		histViewport.y += widgetRect.y;
		SDL_RenderSetViewport(renderer, &histViewport); {
			render_history();
		}
		SDL_RenderSetViewport(renderer, &widgetRect);
	}
	else {
		widgets["btnExpand"]->Render();
	}
	SDL_RenderSetViewport(renderer, &viewPort);
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

		curMessage->render(chatRect.w, chatRect.h - heightUsed);
		heightUsed += curMessage->textHeight;
		
		if (heightUsed >= chatRect.h) break;
	}
}

void CChat::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e); //Mouseover
	
	if (!Expanded) {
		MouseOver = false;
		btnExpand->HandleEvent(e);
		if (btnExpand->IsMouseOver()) MouseOver = true;
		return;
	}

	if (chatField->IsFocus()) {
		if (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT) {
			chatField->HandleEvent(e);
			return;
		}
	}
	else {
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_RETURN2) {
				Window->FocusWidget(chatField);
				return;
			}
		}
	}

	SDL_Event e2 = e;
	e2.motion.x -= X;
	e2.motion.y -= Y;
	if (e.type == SDL_MOUSEMOTION) {
		//Internal mouseover handling
		bool WidgetMouseOver = false;

		if (Expanded) {
			for (auto widget : widgets) {
				if (widget.second->Name != "btnExpand") {
					widget.second->HandleEvent(e2);
					if (widget.second->IsMouseOver()) WidgetMouseOver = true;
				}
			}
		}

		if (MouseOver) {
			MouseOver = false;
			if (Expanded) {
				if (doesPointIntersect(frameRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
					MouseOver = true;
				}
				MouseOver = WidgetMouseOver ? WidgetMouseOver : MouseOver;
			}
		}

		if (held) {
			if (!Dragging) {
				//Delta must be exceeded to start dragging. Prevent slight pixels movement from causing drag.
				if (abs(DragStart.x + DragStart.w - e.motion.x) > DragDelta || abs(DragStart.y + DragStart.h - e.motion.y) > DragDelta) {
					Dragging = true;
				}
			}

			if (Dragging) {
				int maxX = Window->GetWidth() - Width;
				int maxY = Window->GetHeight() - Height - 2;

				int newX = e.motion.x - DragStart.w;
				if (newX < -3) newX = -3;
				if (newX > maxX) newX = maxX;
				int newY = e.motion.y - DragStart.h;
				if (newY < 0) newY = 0;
				if (newY > maxY) newY = maxY;
				SetX(newX);
				SetY(newY);
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (MouseOver) {
			held = true;
			DragStart = { X, Y, e.motion.x - X, e.motion.y - Y };
		}
		else chatField->LoseFocus();
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		held = false;
		Dragging = false;
	}

	
	if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
		BlockMouse = false;
	}
	if (e.type != SDL_MOUSEMOTION) {
		for (auto widget : widgets) {
			widget.second->HandleEvent(e2);
			if (widget.second->IsMouseOver()) BlockMouse = true;
		}
	}

	if (!BlockMouse) {
		if (e.type == SDL_MOUSEBUTTONDOWN) chatField->LoseFocus();
	}
}

void CChat::step() {
	Message message = messageManager.Poll();
	if (message.get()) AddMessage(message);
}

/* Event hooks go here */

CButton* CChat::AddButton(std::string name, int btnW, int btnH, std::string btnPath, std::string btnDownPath, int toX) {
	CButton* newBtn = new CButton(Window, name, toX, 0);
	newBtn->SetWidth(btnW);
	newBtn->SetHeight(btnH);
	newBtn->SetUnPressedImage(btnPath);
	newBtn->SetPressedImage(btnDownPath);
	widgets[name] = newBtn;
	return newBtn;
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

void CChat::SetHeightInLines(int lines) {
	SetHeight(((TTF_FontHeight(font) + LineSpacer) * lines) + HeaderHeight + InputHeight);
}

void CChat::SetPos(SDL_Point pos) {
	x = pos.x;
	y = pos.y;
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

void CChat::ToggleFilter(std::string btnName, ChatFilter newFilter) {
	CButton* newBtn = (CButton*)widgets.find(btnName)->second;
	if (!newBtn || toggledButton == newBtn) return;

	if (toggledButton) toggledButton->Toggle(false, false);
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
		event.user.data2 = Window;
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
		//Restrict to PM/GM?
		//chatField->SetText("");
		//return;
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

void CChat::btnAll_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnAll"])->GetToggled()) ((CButton*)widgets["btnAll"])->Toggle(true, false);
	ToggleFilter("btnAll", cfAll);
}

void CChat::btnLocal_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnLocal"])->GetToggled()) ((CButton*)widgets["btnLocal"])->Toggle(true, false);
	ToggleFilter("btnLocal", cfLocal);
	SetChannel(ccNormal);
}

void CChat::btnWhisper_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnWhisper"])->GetToggled()) ((CButton*)widgets["btnWhisper"])->Toggle(true, false);
	ToggleFilter("btnWhisper", cfWhisper);
	SetChannel(ccPrivate);
}

void CChat::btnTeam_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnTeam"])->GetToggled()) ((CButton*)widgets["btnTeam"])->Toggle(true, false);
	ToggleFilter("btnTeam", cfTeam);
	SetChannel(ccTeam);
}

void CChat::btnFriend_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnFriend"])->GetToggled()) ((CButton*)widgets["btnFriend"])->Toggle(true, false);
	ToggleFilter("btnFriend", cfFriend);
	SetChannel(ccFriend);
}

void CChat::btnGuild_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnGuild"])->GetToggled()) ((CButton*)widgets["btnGuild"])->Toggle(true, false);
	ToggleFilter("btnGuild", cfGuild);
	SetChannel(ccGuild);
}

void CChat::btnSystem_Toggle(SDL_Event& e) {
	if (!((CButton*)widgets["btnSystem"])->GetToggled()) ((CButton*)widgets["btnSystem"])->Toggle(true, false);
	ToggleFilter("btnSystem", cfSystem);
}

void CChat::btnMinimize_Click(SDL_Event& e) {
	Expanded = false;
	btnExpand->SetVisible(true);
}

void CChat::btnExpand_Click(SDL_Event& e) {
	Expanded = true;
	btnExpand->SetVisible(false);
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
	if (renderString.length() == 0) return;
	//Luma of font color dictates a dark or bright shadow
	SDL_Color shadow;
	int luma = (withColor.r * 2 + withColor.b + withColor.g * 3) / 6;
	if (luma > 50) shadow = { 32, 32, 32, 192 };
	else shadow = { 192, 192, 192, 192 };

	std::wstring wstr = StringToWString(renderString);
	if (renderString.length() > 0 && wstr.length() == 0) {
		wstr = L"[Error Converting String into WString]";
		gClient.addToDebugLog("Failed to convert Message String into WString: " + renderString);
	}

	//SDL_Surface *textSurface = TTF_RenderText_Blended(gui->chatFont, renderString.c_str(), withColor);
	SDL_Surface *textSurface = TTF_RenderUNICODE_Solid(gui->fontUni, (const Uint16*)wstr.c_str(), withColor);
	//SDL_Surface *shadowSurface = TTF_RenderText_Blended(gui->chatFont, renderString.c_str(), shadow);
	SDL_Surface *shadowSurface = TTF_RenderUNICODE_Solid(gui->fontUni, (const Uint16*)wstr.c_str(), shadow);
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

		//SDL_RenderCopy(renderer, shadowTexture, &srcRect, &destRect);
		
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
	SDL_Rect textRect{ X, Y + ((Height / 2) - (fontRect.h / 2)), fontRect.w, fontRect.h };
	if (Text.length() == 0) {
		if (hintTexture) {
			SDL_Rect hintRect2 = { X + 5, Y + ((Height / 2) - (hintRect.h / 2)), hintRect.w, hintRect.h };
			SDL_RenderCopy(renderer, hintTexture, NULL, &hintRect2);
		}
	}
	else {
		SDL_Rect srcRect;
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


void CChatField::RenderText() {
	CWidget::RenderText();
}