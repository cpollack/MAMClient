#include "stdafx.h"
#include "Chat.h"

#include "MainWindow.h"
#include "Player.h"
#include "pMessage.h"

#include "Field.h"

CChat::CChat(CWindow* window) {
	opacity = 128;

	chatField = new CChatField(window, this, "chatField", 0, 0);
	chatField->SetHeight(InputHeight);
	widgets["chatField"] = chatField;
	registerEvent("chatField", "Submit", std::bind(&CChat::chatField_Submit, this, std::placeholders::_1));

	UpdateRect();
	chatColor = { 255, 255, 255, opacity };
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
	//roundedBoxRGBA(renderer, 0, 0, mainRect.w, mainRect.h, 10, 0, 0, 0, opacity);

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
	chatField->Render();

	SDL_DestroyTexture(histTexture); //this breaks rectangleRGBA for some reason

	SDL_SetRenderTarget(renderer, priorTarget);
	SDL_RenderCopy(renderer, mainTexture, NULL, &mainRect);
	SDL_DestroyTexture(mainTexture);
}

void CChat::render_history() {
	int heightUsed = 0;
	for (int i = messages.size(); i > 0; i--) {
		CMessage *curMessage = messages[i - 1];
		if (curMessage->channel == ccHidden) continue;
		curMessage->render(width, histRect.h - heightUsed);
		heightUsed += curMessage->textHeight;
		
		if (heightUsed >= histRect.h) break;
	}
}

void CChat::handleEvent(SDL_Event& e) {
	//chatField->HandleEvent(e);

	for (auto widget : widgets) widget.second->HandleEvent(e);
}

void CChat::registerEvent(std::string widgetName, std::string eventName, EventFunc evf) {
	auto iter = widgets.find(widgetName);
	if (iter != widgets.end()) {
		iter->second->RegisterEvent(eventName, evf);
	}
}

/* Event hooks go here */

void CChat::UpdateRect() {
	mainRect = { x, y, width, height };
	headerRect = { 0, 0, width, 13 };
	chatRect = { 0, height - InputHeight, width, InputHeight };
	histRect = { 0, headerRect.h, width, height - headerRect.h - chatRect.h };
	scrollRect = { width - 8, histRect.y, 8, histRect.h };

	chatField->SetX(chatRect.x + 5);
	chatField->SetY(chatRect.y);
	chatField->SetWidth(chatRect.w - 10);
}

void CChat::SetRenderer(SDL_Renderer* r) {
	renderer = r;
	//chatField->SetRenderer(renderer);
	UpdateRect();
	//chatField->SetText("Test Text!");
	chatField->SetHint("Talking in [Normal]");
}

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
	SetHeight(TTF_FontHeight(font) * lines);
}

void CChat::SetPos(SDL_Point pos) {
	x = pos.x;
	y = pos.y;
	UpdateRect();
}

void CChat::AddMessage(pMessage *packet) {
	CMessage *newMessage = new CMessage(renderer, width, packet);
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

void CChat::chatField_Submit(SDL_Event& e) {
	std::string chatText = chatField->GetText();
	if (chatText.length() == 0) return;

	int iColor = ((chatColor.r & 0xFF) << 16) | ((chatColor.g & 0xFF) << 8) | (chatColor.b & 0xFF);
	pMessage *sendMessage = new pMessage(channel, player->GetName(), target, chatText, iColor, effect, emotion);
	AddMessage(sendMessage);
	gClient.addPacket(sendMessage);
	chatField->SetText("");
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
			if (channel == ccSystem) channelColor = { 255, 0, 255, 255 }; //pruple
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

	SDL_Surface *textSurface = TTF_RenderText_Blended(gui->chatFont, renderString.c_str(), withColor);
	SDL_Surface *shadowSurface = TTF_RenderText_Blended(gui->chatFont, renderString.c_str(), shadow);
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
		SDL_SetTextureBlendMode(shadowTexture, SDL_BlendMode::SDL_BLENDMODE_NONE);

		SDL_RenderCopy(renderer, shadowTexture, &srcRect, &destRect);
		
		destRect.x -= 1;
		destRect.y -= 1;
		SDL_RenderCopy(renderer, textTexture, &srcRect, &destRect);
		SDL_RenderCopy(renderer, textTexture, &srcRect, &destRect);

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
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		mx -= (gui->left->width + 20 + pChat->GetX());
		my -= (gui->topCenter->height + 9 + pChat->GetY());

		if (DoesPointIntersect(SDL_Point{ mx, my })) {
			held = true;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		mx -= (gui->left->width + 20 + pChat->GetX());
		my -= (gui->topCenter->height + 9 + pChat->GetY());

		if (held && DoesPointIntersect(SDL_Point{ mx, my })) {
			OnClick(e);
			return;
		}

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
