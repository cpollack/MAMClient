#include "stdafx.h"
#include "PlayerInfoFrame.h"

#include "CharacterForm.h"
#include "PetListForm.h"
#include "Window.h"
#include "Gauge.h"
#include "Label.h"

#include "Battle.h"
#include "Player.h"
#include "Pet.h"

CPlayerInfoFrame::CPlayerInfoFrame(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	SetX(x);
	SetY(y);
	SetWidth(250);
	SetHeight(170);

	lblPlayerLevel = new CLabel(window, "lblPlayerLevel", 55, 110);
	lblPetLevel = new CLabel(window, "lblPetLevel", 14, 43);
	lblPlayerLevel->SetAlignment(laCenter);
	lblPlayerLevel->SetVAlignment(lvaCenter);
	lblPlayerLevel->SetWidth(32);
	lblPlayerLevel->SetHeight(15);
	lblPetLevel->SetAlignment(laCenter);
	lblPetLevel->SetVAlignment(lvaCenter);
	lblPetLevel->SetWidth(32);
	lblPetLevel->SetHeight(15);

	btnHide = new CButton(window, "btnHide", 233, 42);
	btnHide->SetWidth(18);
	btnHide->SetHeight(70);
	btnHide->SetUnPressedImage("data/GUI/main/player_collapse.png");
	btnHide->SetPressedImage("data/GUI/main/player_expand.png");
	btnHide->SetType(btToggle);
	widgets["btnHide"] = btnHide;
	RegisterEvent("btnHide", "Toggle", std::bind(&CPlayerInfoFrame::btnHide_Toggle, this, std::placeholders::_1));

	Load();

	window->AddWidget(this);
}

CPlayerInfoFrame::~CPlayerInfoFrame() {
	//free gauges
	Cleanup();

	if (gaugePlayerHealth) delete gaugePlayerHealth;
	if (gaugePlayerMana) delete gaugePlayerMana;
	if (gaugePlayerExp) delete gaugePlayerExp;
	if (gaugePetHealth) delete gaugePetHealth;
	if (gaugePetExp) delete gaugePetExp;
	delete lblPlayerLevel;
	delete lblPetLevel;
	delete btnHide;
}

void CPlayerInfoFrame::Cleanup() {
	if (playerPixels) delete[] playerPixels;
	if (petPixels) delete[] petPixels;
}

void CPlayerInfoFrame::Load() {
	Cleanup();

	//Player Frame
	PlayerFrame.reset(new Texture(renderer));

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	PlayerFrame->texture = texture;
	PlayerFrame->width = Width;
	PlayerFrame->height = Height;

	if (Expanded) {
		MergePiece(PlayerFrame, "player_gauge.png", 90, 38);
	}
	else {
		MergePiece(PlayerFrame, "player_gauge_small.png", 64, 38);
	}

	MergePiece(PlayerFrame, "player_circle.png", 21, 30);
	int offset = 5;
	PlayerBtnRect = { 21 + offset, 30 + offset, PlayerWidth - (offset*2), PlayerWidth - (offset*2) };

	//Player Portrait
	int face = player->GetFace() - 1;
	if (player->GetGender() == FEMALE) face += 8;
	std::string strFace = std::to_string(face);
	while (strFace.length() < 3) strFace.insert(strFace.begin(), '0');
	playerPortrait.reset();
	LoadPortrait(playerPortrait, "data/face/player/" + strFace + ".tga", PlayerWidth, true);
	MergePiece(PlayerFrame, playerPortrait, 31, 37);

	MergePiece(PlayerFrame, "player_level.png", 55, 109);
	//MergePiece(PlayerFrame, "player_collapse.png", 233, 42);

	//lookup rank here
	MergePiece(PlayerFrame, "player_rank.png", 0, 0);

	SDL_Texture *priorTarget;
	priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, PlayerFrame->texture); {
		int pitch = Width * 4;
		playerPixels = new Uint32[Width * Height];
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, playerPixels, pitch);
	}
	SDL_SetRenderTarget(renderer, priorTarget);
	PlayerFrame->setBlendMode(SDL_BLENDMODE_BLEND);

	if (!gaugePlayerHealth) gaugePlayerHealth = addGauge("gaugePlayerHealth", 115, 44, 120, 20, "player_life.png");
	if (!gaugePlayerMana) gaugePlayerMana = addGauge("gaugePlayerMana", 115, 67, 120, 20, "player_mana.png");
	if (!gaugePlayerExp) gaugePlayerExp = addGauge("gaugePlayerExp", 115, 89, 120, 20, "player_exp.png");
	updatePlayerLevel();
	
	//Pet Frame
	PetFrame.reset(new Texture(renderer));

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PetFrameWidth, PetFrameHeight);
	PetFrame->texture = texture;
	PetFrame->width = PetFrameWidth;
	PetFrame->height = PetFrameHeight;

	if (Expanded) MergePiece(PetFrame, "pet_gauge.png", 28, 9);
	MergePiece(PetFrame, "pet_circle.png", 0, 0);
	offset = 3;
	PetBtnRect = { petOffset.x + 13 + offset, petOffset.y + 9 + offset, PetWidth - (offset * 2), PetWidth - (offset * 2) };

	Pet *pPet = player->GetMarchingPet();
	if (pPet) {
		std::string look = std::to_string(pPet->GetLook());
		while (look.size() < 4) look.insert(look.begin(), '0');
		look = "peticon" + look;

		INI ini("INI\\petLook.ini", look);
		std::string iconPath = ini.getEntry("Frame0");

		petPortait.reset(new Texture(renderer, iconPath));
		LoadPortrait(petPortait, "", PetWidth, false);
		MergePiece(PetFrame, petPortait, 7, 9);
	}

	MergePiece(PetFrame, "player_level.png", 14, 42);

	SDL_SetRenderTarget(renderer, PetFrame->texture); {
		int pitch = PetFrameWidth * 4;
		petPixels = new Uint32[PetFrameWidth * PetFrameHeight];
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, petPixels, pitch);
	}
	SDL_SetRenderTarget(renderer, priorTarget);
	PetFrame->setBlendMode(SDL_BLENDMODE_BLEND);

	if (!gaugePetHealth) gaugePetHealth = addGauge("gaugePetHealth", 56, 15, 74, 14, "pet_life.png");
	if (!gaugePetExp) gaugePetExp = addGauge("gaugePetExp", 56, 29, 74, 14, "pet_exp.png");
	updatePetLevel();
}

void CPlayerInfoFrame::MergePiece(Asset asset, std::string fileName, int x, int y) {
	std::string prefix = "data/GUI/Main/";
	Asset piece(new Texture(renderer, prefix + fileName));
	MergePiece(asset, piece, x, y);
}

void CPlayerInfoFrame::MergePiece(Asset asset, Asset piece, int x, int y) {
	piece->setBlendMode(SDL_BLENDMODE_BLEND);
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, asset->texture);
	SDL_Rect destRect = { x, y, piece->width, piece->height };
	SDL_RenderCopy(renderer, piece->texture, NULL, &destRect);
	SDL_SetRenderTarget(renderer, priorTarget);
}

void CPlayerInfoFrame::ReloadAssets () {
	Load();
}

void CPlayerInfoFrame::Render() {
	SDL_Rect playerRect = { X, Y, PlayerFrame->width, PlayerFrame->height };
	SDL_RenderCopy(renderer, PlayerFrame->texture, NULL, &playerRect);

	SDL_Rect viewPort;
	SDL_RenderGetViewport(renderer, &viewPort);
	SDL_RenderSetViewport(renderer, &playerRect);
	{
		gaugePlayerHealth->Render();
		gaugePlayerMana->Render();
		gaugePlayerExp->Render();
		lblPlayerLevel->Render();
		btnHide->Render();

		if (PetFrame.get()) {
			SDL_Rect petRect = {petOffset.x, petOffset.y, PetFrame->width, PetFrame->height };
			SDL_RenderCopy(renderer, PetFrame->texture, NULL, &petRect);
			petRect.x += X; petRect.y += Y;
			SDL_RenderSetViewport(renderer, &petRect);
			{
				gaugePetHealth->Render();
				gaugePetExp->Render();
				lblPetLevel->Render();
			}
		}
	}
	SDL_RenderSetViewport(renderer, &viewPort);
}

void CPlayerInfoFrame::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e); //Mouseover

	SDL_Event e2 = e;
	e2.motion.x -= X;
	e2.motion.y -= Y;

	if (e.type == SDL_MOUSEMOTION) {
		MouseOverPlayer = false;
		MouseOverPet = false;
		btnHide->HandleEvent(e2);

		if (MouseOver) {
			MouseOver = false;
			SDL_Point point = { e.motion.x - X, e.motion.y - Y };
			SDL_Point petPoint = { point.x - petOffset.x, point.y - petOffset.y };
			if (IsAssetPixel(PlayerFrame, playerPixels, point)) {
				MouseOver = true;
				MouseOverPlayer = true;
			}
			else if (IsAssetPixel(PetFrame, petPixels, petPoint)) {
				MouseOver = true;
				MouseOverPet = true;
			}
			else if(btnHide->IsMouseOver()) MouseOver = true;
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
				if (!Expanded) maxX += 112;
				int maxY = Window->GetHeight() - Height;

				int newX = e.motion.x - DragStart.w;
				if (newX < 0) newX = 0;
				if (newX > maxX) newX = maxX;
				int newY = e.motion.y - DragStart.h;
				if (newY < 0) newY = 0;
				if (newY > maxY) newY = maxY;
				SetX(newX);
				SetY(newY);
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN && MouseOver) {
		held = true;
		DragStart = { X, Y, e.motion.x - X, e.motion.y - Y };
		btnPlayer_Down = false;
		btnPet_Down = false;
		btnHide->HandleEvent(e2);

		if (MouseOverPlayer && doesPointIntersect(PlayerBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
			btnPlayer_Down = true;
		}
		else if (MouseOverPet && doesPointIntersect(PetBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
			btnPet_Down = true;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		if (!Dragging) {
			btnHide->HandleEvent(e2);
			if (MouseOverPlayer && doesPointIntersect(PlayerBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
				btnCharacter_Click(e);
			}
			else if (MouseOverPet && doesPointIntersect(PetBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
				btnPet_Click(e);
			}
		}
		held = false;
		btnPlayer_Down = false;
		btnPet_Down = false;
		Dragging = false;
	}
}

CGauge* CPlayerInfoFrame::addGauge(std::string name, int x, int y, int w, int h, std::string foreground) {
	std::string prefix = "data/GUI/Main/";

	CGauge *gauge = new CGauge(Window, name, x, y);
	gauge->SetWidth(w);
	gauge->SetHeight(h);
	gauge->SetForegroundImage(prefix + foreground);
	return gauge;
}

void CPlayerInfoFrame::LoadPortrait(Asset &portrait, std::string path, int width, bool bFlip) {
	SDL_Texture *priorTarget;
	priorTarget = SDL_GetRenderTarget(renderer);
	SDL_Texture* texture;
	bool isPet = !bFlip; //Hack, flip is only used for player

	if (!portrait.get()) portrait.reset(new Texture(renderer, path));
	if (!portrait.get()) return;
		
	//Shrink and flip if appropriate
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, width);
	SDL_SetRenderTarget(renderer, texture);
	
	SDL_Rect destRec = { 0, 0, width, width };
	if (portrait->width != portrait->height) {
		//Compensate for smaller width, and offset x position to center
		double dif = (double)portrait->width / portrait->height;
		destRec.w *= dif;
		destRec.x = (width - destRec.w) / 2;
	}

	SDL_RendererFlip flip = (bFlip) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RenderCopyEx(renderer, portrait->texture, NULL, &destRec, 0, NULL, flip);
	SDL_DestroyTexture(portrait->texture);
	portrait->texture = texture;
	portrait->width = width;
	portrait->height = width;

	//Generate masking texture
	Asset mask(new Texture(renderer));
	SDL_Texture* tMask = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, width);
	mask->texture = tMask;
	mask->width = width;
	mask->height = width;
	SDL_SetRenderTarget(renderer, tMask);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	filledCircleRGBA(renderer, width / 2, width / 2, width / 2, 255, 255, 255, 255);	

	//Get the mask and texture pixel data
	int pitch = width * 4;
	Uint32 *maskPixels = new Uint32[80 * 80], *texturePixels = new Uint32[80*80];
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, maskPixels, pitch);
	SDL_SetRenderTarget(renderer, texture);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA8888, texturePixels, pitch);
	
	//mask the bottom left quadrant
	if (!isPet) {
		for (int x = 0; x < width / 2; x++) {
			for (int y = width / 2; y < width; y++) {
				int pos = y * width + x;
				Uint32 pixel = maskPixels[pos];

				//clear pixels not on mask
				if (pixel == 0) texturePixels[pos] = 0;
			}
		}
	}
	else {
		//Mask entire image
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < width; y++) {
				int pos = y * width + x;
				Uint32 pixel = maskPixels[pos];

				//clear pixels not on mask
				if (pixel == 0) texturePixels[pos] = 0;
			}
		}
	}
	SDL_UpdateTexture(texture, NULL, texturePixels, pitch);
	delete[] maskPixels;
	delete[] texturePixels;

	SDL_SetRenderTarget(renderer, priorTarget);
}

bool CPlayerInfoFrame::IsAssetPixel(Asset asset, Uint32 *pixels, SDL_Point point) {
	if (!asset || !pixels) return false;
	if (point.x < 0 || point.y < 0) return false;
	if (point.x > asset->width || point.y > asset->height) return false;

	int idx = asset->width * point.y + point.x;
	if (pixels[idx] > 0) return true;
	return false;
}

void CPlayerInfoFrame::btnCharacter_Click(SDL_Event& e) {
	if (battle) return;
	CCharacterForm* charForm = new CCharacterForm();
	Windows.push_back(charForm);
}

void CPlayerInfoFrame::btnPet_Click(SDL_Event& e) {
	if (battle) return;
	CPetListForm* petForm = new CPetListForm();
	Windows.push_back(petForm);
}

void CPlayerInfoFrame::btnHide_Toggle(SDL_Event& e) {
	if (btnHide->GetToggled()) {
		Expanded = false;
		btnHide->SetPosition(120, 42);
		gaugePlayerHealth->SetVisible(false);
		gaugePlayerMana->SetVisible(false);
		gaugePlayerExp->SetVisible(false);
		gaugePetHealth->SetVisible(false);
		gaugePetExp->SetVisible(false);
	}
	else {
		Expanded = true;
		btnHide->SetPosition(233, 42);
		gaugePlayerHealth->SetVisible(true);
		gaugePlayerMana->SetVisible(true);
		gaugePlayerExp->SetVisible(true);
		gaugePetHealth->SetVisible(true);
		gaugePetExp->SetVisible(true);

		int maxX = Window->GetWidth() - Width;
		if (X > maxX) SetX(maxX);
	}
	Load();
}

void CPlayerInfoFrame::setPlayerHealthGauge(int val) {
	gaugePlayerHealth->set(val);
}


void CPlayerInfoFrame::setPlayerHealthGauge(int val, int max) {
	gaugePlayerHealth->set(val, max);
}


void CPlayerInfoFrame::shiftPlayerHealthGauge(int val) {
	if (val >= 0) gaugePlayerHealth->add(val);
	else gaugePlayerHealth->subtract(val);
}


void  CPlayerInfoFrame::adjustPlayerHealthGauge(int val) {
	gaugePlayerHealth->AdjustTo(val);
}


void CPlayerInfoFrame::setPlayerManaGauge(int val) {
	gaugePlayerMana->set(val);
}


void CPlayerInfoFrame::setPlayerManaGauge(int val, int max) {
	gaugePlayerMana->set(val, max);
}


void CPlayerInfoFrame::shiftPlayerManaGauge(int val) {
	if (val >= 0) gaugePlayerMana->add(val);
	else gaugePlayerMana->subtract(val);
}


void  CPlayerInfoFrame::adjustPlayerManaGauge(int val) {
	gaugePlayerMana->AdjustTo(val);
}


void CPlayerInfoFrame::setPlayerExpGauge(int val) {
	gaugePlayerExp->set(val);
}


void CPlayerInfoFrame::setPlayerExpGauge(int val, int max) {
	gaugePlayerExp->set(val, max);
}


void CPlayerInfoFrame::shiftPlayerExpGauge(int val) {
	if (val >= 0) gaugePlayerExp->add(val);
	else gaugePlayerExp->subtract(val);
}


void  CPlayerInfoFrame::adjustPlayerExpGauge(int val) {
	gaugePlayerExp->AdjustTo(val);
}


void CPlayerInfoFrame::setPetHealthGauge(int val) {
	gaugePetHealth->set(val);
}


void CPlayerInfoFrame::setPetHealthGauge(int val, int max) {
	gaugePetHealth->set(val, max);
}


void CPlayerInfoFrame::shiftPetHealthGauge(int val) {
	if (val >= 0) gaugePetHealth->add(val);
	else gaugePetHealth->subtract(val);
}


void  CPlayerInfoFrame::adjustPetHealthGauge(int val) {
	gaugePetHealth->AdjustTo(val);
}


void CPlayerInfoFrame::setPetExpGauge(int val) {
	gaugePetExp->set(val);
}


void CPlayerInfoFrame::setPetExpGauge(int val, int max) {
	gaugePetExp->set(val, max);
}


void CPlayerInfoFrame::shiftPetExpGauge(int val) {
	if (val >= 0) gaugePetExp->add(val);
	else gaugePetExp->subtract(val);
}


void  CPlayerInfoFrame::adjustPetExpGauge(int val) {
	gaugePetExp->AdjustTo(val);
}


void CPlayerInfoFrame::updatePlayerLevel() {
	lblPlayerLevel->SetText(std::to_string(player->GetLevel()));
}

void CPlayerInfoFrame::updatePetLevel() {
	Pet *pPet = player->GetMarchingPet();
	if (pPet) {
		lblPetLevel->SetText(std::to_string(pPet->GetLevel()));
	}
	else lblPetLevel->SetText("");
}