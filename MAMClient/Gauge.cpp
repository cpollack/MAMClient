#include "stdafx.h"
#include "Gauge.h"
#include "Define.h"

CGauge::CGauge(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	SetPosition(SDL_Point{ x,y });

	//fontColor = gui->buttonFontColor;
	
	Max = 0;
	Current = 0;
	fillPerc = 0;
}


CGauge::CGauge(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("ShowLabel")) ShowLabel = vWidget["ShowLabel"].GetBool();
}


CGauge::~CGauge() {
	if (backgroundImage) delete backgroundImage;
	else if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
	if (foregroundImage) delete foregroundImage;
	else if (foregroundTexture) SDL_DestroyTexture(foregroundTexture);
	if (decreaseImage) delete decreaseImage;
	else if (decreaseTexture) SDL_DestroyTexture(decreaseTexture);
	if (increaseImage) delete increaseImage;
	else if (increaseTexture) SDL_DestroyTexture(increaseTexture);
}

void CGauge::ReloadAssets() {
	CreateGaugeTexture();
}

void CGauge::Render() {
	if (!Visible) return;
	if (!Loaded) CreateGaugeTexture();

	if (usingImages) Render_Textures();
	else Render_Draw();
}

void CGauge::Render_Textures() {
	SDL_RenderCopy(renderer, backgroundTexture, NULL, &widgetRect);

	if (shiftDown != -1) {
		if (Verticle) {
			float fillHeight = Height * fillPerc;
			elapsedTime = SDL_GetTicks() - startTime;
			float excess = Height * ((float)(shiftDown - Current) / (float)Max) *  ((float)(shiftSpeed - elapsedTime) / (float)shiftSpeed);

			if (decreaseTexture) {
				SDL_Rect shiftRect_src = { 0, Height - (fillHeight + excess), Width, (fillHeight + excess) };
				SDL_Rect shiftRect_dest = { X, Y + shiftRect_src.y, shiftRect_src.w, shiftRect_src.h };
				SDL_RenderCopy(renderer, decreaseTexture, &shiftRect_src, &shiftRect_dest);

				SDL_Rect fgRect_src = { 0, Height - fillHeight, Width, fillHeight };
				SDL_Rect fgRect_dest = { X, Y + fgRect_src.y, fgRect_src.w, fgRect_src.h };
				SDL_RenderCopy(renderer, foregroundTexture, &fgRect_src, &fgRect_dest);
			}
			else {
				SDL_Rect shiftRect_src = { 0, Height - (fillHeight + excess), Width, (fillHeight + excess) };
				SDL_Rect shiftRect_dest = { X, Y + shiftRect_src.y, shiftRect_src.w, shiftRect_src.h };
				SDL_RenderCopy(renderer, foregroundTexture, &shiftRect_src, &shiftRect_dest);
			}
		}
		else { //Horizontal
			float fillWidth = Width * fillPerc;
			elapsedTime = SDL_GetTicks() - startTime;
			float excess = Width * ((float)(shiftDown - Current) / (float)Max) *  ((float)(shiftSpeed - elapsedTime) / (float)shiftSpeed);

			if (decreaseTexture) {
				SDL_Rect shiftRect_src = { 0, 0, fillWidth + excess, Height };
				SDL_Rect shiftRect_dest = { X, Y, shiftRect_src.w, shiftRect_src.h };
				SDL_RenderCopy(renderer, decreaseTexture, &shiftRect_src, &shiftRect_dest);

				SDL_Rect fgRect_src = { 0, 0, fillWidth, Height };
				SDL_Rect fgRect_dest = { X, Y, fgRect_src.w, fgRect_src.h };
				SDL_RenderCopy(renderer, foregroundTexture, &fgRect_src, &fgRect_dest);
			}
			else {
				if (RightFill) {
					SDL_Rect shiftRect_src = { 0, 0, fillWidth + excess, Height };
					SDL_Rect shiftRect_dest = { X, Y, shiftRect_src.w, shiftRect_src.h };
					SDL_RenderCopy(renderer, foregroundTexture, &shiftRect_src, &shiftRect_dest);
				}
				else {
					SDL_Rect shiftRect_src = { 0, 0, fillWidth + excess, Height };
					SDL_Rect shiftRect_dest = { X, Y, shiftRect_src.w, shiftRect_src.h };
					SDL_RenderCopy(renderer, foregroundTexture, &shiftRect_src, &shiftRect_dest);
				}
			}
		}

		if (elapsedTime >= shiftSpeed) shiftDown = -1;
		return;
	}

	if (shiftUp != -1) {
		if (Verticle) {
			float fillHeight = Height * fillPerc;
			elapsedTime = SDL_GetTicks() - startTime;
			float elapsedPerc = (float)elapsedTime / (float)shiftSpeed;
			if (elapsedPerc > 1) elapsedPerc = 1.0;
			float excess = Width * (((float)(Current - shiftUp) / (float)Max) *  elapsedPerc);

			if (increaseTexture) {
				SDL_Rect shiftRect_src = { 0, Height - fillHeight, Width, fillHeight };
				SDL_Rect shiftRect_dest = { X, Y + shiftRect_src.y, shiftRect_src.w, shiftRect_src.h };
				SDL_RenderCopy(renderer, increaseTexture, &shiftRect_src, &shiftRect_dest);

				int adjH = ((float)shiftUp / (float)Max * Height) + excess;
				SDL_Rect fgRect_src = { 0, Height - adjH, Width,  adjH };
				SDL_Rect fgRect_dest = { X, Y + fgRect_src.y, fgRect_src.w, fgRect_src.h };
				SDL_RenderCopy(renderer, foregroundTexture, &fgRect_src, &fgRect_dest);
			}
			else {
				SDL_Rect shiftRect_src = { 0, Height - fillHeight, Width, fillHeight };
				SDL_Rect shiftRect_dest = { X, Y + shiftRect_src.y, shiftRect_src.w, shiftRect_src.h };
				SDL_RenderCopy(renderer, foregroundTexture, &shiftRect_src, &shiftRect_dest);
			}
		}
		else {
			float fillWidth = Width * fillPerc;
			elapsedTime = SDL_GetTicks() - startTime;
			float elapsedPerc = (float)elapsedTime / (float)shiftSpeed;
			if (elapsedPerc > 1) elapsedPerc = 1.0;
			float excess = Width * (((float)(Current - shiftUp) / (float)Max) *  elapsedPerc);

			if (increaseTexture) {
				SDL_Rect shiftRect_src = { 0, 0, fillWidth, Height };
				SDL_Rect shiftRect_dest = { X, Y, shiftRect_src.w, shiftRect_src.h };
				SDL_RenderCopy(renderer, increaseTexture, &shiftRect_src, &shiftRect_dest);

				SDL_Rect fgRect_src = { 0, 0, ((float)shiftUp / (float)Max * Width) + excess, Height };
				SDL_Rect fgRect_dest = { X, Y, fgRect_src.w, fgRect_src.h };
				SDL_RenderCopy(renderer, foregroundTexture, &fgRect_src, &fgRect_dest);
			}
			else {
				if (RightFill) {
					int val = ((float)shiftUp / (float)Max * Width) + excess;
					SDL_Rect shiftRect_src = { Width - val, 0, val, Height };
					SDL_Rect shiftRect_dest = { X + shiftRect_src.x, Y, shiftRect_src.w, shiftRect_src.h };
					SDL_RenderCopy(renderer, foregroundTexture, &shiftRect_src, &shiftRect_dest);
				}
				else {
					SDL_Rect shiftRect_src = { 0, 0, ((float)shiftUp / (float)Max * Width) + excess, Height };
					SDL_Rect shiftRect_dest = { X, Y, shiftRect_src.w, shiftRect_src.h };
					SDL_RenderCopy(renderer, foregroundTexture, &shiftRect_src, &shiftRect_dest);
				}
			}
		}

		if (elapsedTime >= shiftSpeed) shiftUp = -1;
		return;
	}

	SDL_Rect fgRect_src;
	SDL_Rect fgRect_dest;
	if (Verticle) {
		fgRect_src = { 0, Height - (int)(Height * fillPerc), Width, (int)(Height * fillPerc) };
		fgRect_dest = { X, Y + fgRect_src.y, fgRect_src.w, fgRect_src.h };
	}
	else {
		if (RightFill) {
			fgRect_src = { Width - (int)(Width * fillPerc), 0, (int)(Width * fillPerc), Height };
			fgRect_dest = { X + fgRect_src.x, Y + fgRect_src.y, fgRect_src.w, fgRect_src.h };
		}
		else {
			fgRect_src = { 0, 0, (int)(Width * fillPerc), Height };
			fgRect_dest = { X, Y + fgRect_src.y, fgRect_src.w, fgRect_src.h };
		}
	}
	SDL_RenderCopy(renderer, foregroundTexture, &fgRect_src, &fgRect_dest);
}

void CGauge::Render_Draw() {
	// BG > FG > Text
	SDL_RenderCopy(renderer, backgroundTexture, NULL, &widgetRect);

	SDL_Rect fgRect_src = { 0, 0, ((Width - 4) * fillPerc), Height - 4 };
	SDL_Rect fgRect_dest = { X + 2, Y + 2, fgRect_src.w, fgRect_src.h };
	SDL_RenderCopy(renderer, foregroundTexture, &fgRect_src, &fgRect_dest);

	if (ShowLabel) {
		SDL_Rect textRect = { X, Y, fontRect.w, fontRect.h };
		textRect.x += (Width / 2) - (fontRect.w / 2);
		textRect.y += (Height / 2) - (fontRect.h / 2);
		SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);
	}
}

void CGauge::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e);

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (MouseOver) {
			held = true;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		if (MouseOver) {
			if (held) {
				//
			}
		}
		held = false;
	}
}

void CGauge::CreateGaugeTexture() {
	if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
	if (foregroundTexture) SDL_DestroyTexture(foregroundTexture);
	if (increaseTexture) SDL_DestroyTexture(increaseTexture);
	if (decreaseTexture) SDL_DestroyTexture(decreaseTexture);

	SDL_Texture* priorTarget = SDL_GetRenderTarget(renderer);
	SDL_Color buttonColor = gui->buttonColor;
	/*
	outer: 004080
	inner: 336699
	*/

	SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
	SDL_RenderClear(renderer);

	if (ShowLabel && !fontTexture) RenderText();

	SDL_SetRenderTarget(renderer, NULL);

	SDL_Color bbColor = { 0x00, 0x40, 0x80, 0xFF };
	SDL_Color fillColor = { 0x33, 0x66, 0x99, 0xFF };
	SDL_Rect renderRect = { 0, 0, Width, Height };

	//Background Texture
	if (usingImages) {
		if (backgroundImage) delete backgroundImage;
		if (UseGUI) {
			backgroundImage = gui->getSkinTexture(renderer, backgroundImagePath, Anchor::ANCOR_TOPLEFT);
		}
		else {
			backgroundImage = new Texture(renderer, backgroundImagePath);
		}
		backgroundTexture = backgroundImage->texture;
	}
	else {
		backgroundTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
		SDL_SetTextureBlendMode(backgroundTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, backgroundTexture);
		rectangleRGBA(renderer, 0, 0, Width - 1, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	}

	//Foreground Texture
	if (usingImages) {
		if (foregroundImage) delete foregroundImage;
		if (UseGUI) {
			foregroundImage = gui->getSkinTexture(renderer, foregroundImagePath, Anchor::ANCOR_TOPLEFT);
		}
		else {
			foregroundImage = new Texture(renderer, foregroundImagePath);
		}
		foregroundTexture = foregroundImage->texture;
	}
	else {
		foregroundTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width-4, Height-4);
		SDL_SetTextureBlendMode(foregroundTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, foregroundTexture);
		SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
		SDL_RenderClear(renderer);
	}

	//Increase and Decrease Textures - specific to when using images
	if (usingImages) {
		if (UseGUI) {
			if (increaseImage) delete increaseImage;
			if (increaseImagePath.length()) increaseImage = gui->getSkinTexture(renderer, increaseImagePath, Anchor::ANCOR_TOPLEFT);
			if (increaseImage) increaseTexture = increaseImage->texture;
			if (decreaseImage) delete decreaseImage;
			if (decreaseImagePath.length())decreaseImage = gui->getSkinTexture(renderer, decreaseImagePath, Anchor::ANCOR_TOPLEFT);
			if (decreaseImage) decreaseTexture = decreaseImage->texture;
		}
		else {
			//non gui textures?
		}
	}

	//Mouseover Texture
	/*if (!usingImages) {
		mouseoverTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
		SDL_SetRenderTarget(renderer, mouseoverTexture);
		SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
		hlineRGBA(renderer, 0, Width - 1, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		SDL_SetRenderTarget(renderer, NULL);
	}*/

	SDL_DestroyTexture(mainTexture);

	SDL_SetRenderTarget(renderer, priorTarget);
	Loaded = true;
}

void CGauge::SetForegroundImage(std::string imagePath) {
	foregroundImagePath = imagePath;
	usingImages = true;
}

void CGauge::SetBackgroundImage(std::string imagePath) {
	backgroundImagePath = imagePath;
	usingImages = true;
}

void CGauge::SetDecreaseImage(std::string imagePath) {
	decreaseImagePath = imagePath;
	usingImages = true;
}

void CGauge::SetIncreaseImage(std::string imagePath) {
	increaseImagePath = imagePath;
	usingImages = true;
}

void CGauge::SetUseGUI(bool use) {
	UseGUI = use;
}

void CGauge::set(int val) {
	Current = val;
	shiftUp = -1;
	shiftDown = -1;
	fillPerc = (float)Current / (float)Max;
	if (fillPerc > 1) fillPerc = 1;
	if (ShowLabel) updateLabel();
}


void CGauge::set(int val, int max) {
	Current = val;
	this->Max = max;
	shiftUp = -1;
	shiftDown = -1;
	fillPerc = (float)Current / (float)max;
	if (fillPerc > 1) fillPerc = 1;
	if (ShowLabel) updateLabel();
}


void CGauge::add(int val) {
	if (Current == Max) return;
	shiftUp = Current;
	Current += val;
	if (Current > Max) Current = Max;
	fillPerc = (float)Current / (float)Max;
	if (fillPerc > 1) fillPerc = 1;
	startTime = SDL_GetTicks();
	elapsedTime = 0;
	if (ShowLabel) updateLabel();
}


void CGauge::subtract(int val) {
	if (Current == 0) return;
	shiftDown = Current;
	Current -= val;
	if (Current < 0) Current = 0;
	fillPerc = (float)Current / (float)Max;
	if (fillPerc > 1) fillPerc = 1;
	startTime = SDL_GetTicks();
	elapsedTime = 0;
	if (ShowLabel) updateLabel();
}

void CGauge::AdjustTo(int val) {
	int dif = Current - val;
	if (dif > 0) subtract(dif);
	if (dif < 0) add(abs(dif));
}


void CGauge::updateLabel() {
	if (CustomLabel) return;
	std::string labelText;
	labelText = std::to_string(Current) + "/" + std::to_string(Max);
	SetText(labelText);
}

void CGauge::SetLabel(std::string label) {
	SetText(label);
}