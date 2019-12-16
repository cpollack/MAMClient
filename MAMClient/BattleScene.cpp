#include "stdafx.h"
#include "BattleScene.h"
#include "Battle.h"

BattleScene::BattleScene(std::vector<bAlly*> allies, std::vector<bMonster*> monsters, std::vector<FloatingLabel*> *fLabels) {
	cast = monsters;
	cast.insert(cast.end(), allies.begin(), allies.end());
	floatingLabels = fLabels;
}


BattleScene::~BattleScene() {
	for (auto actor : actors) {
		delete actor;
	}
}


void BattleScene::setReactor(bMonster* target, int action) {
	reactor = target;
	reaction = action;
}


void BattleScene::addAction(bMonster* source, bsType type, AnimType action, SDL_Point target, FloatingLabel *triggerLabel) {
	bsPerform* performance = new bsPerform;
	performance->actor = source;
	performance->type = type;
	performance->animation = action;
	performance->targetPoint = target;
	if (type == bsMoveTo) performance->direction = getDirection(source->pos, target);
	else performance->direction = source->direction;
	performance->triggerLabel = triggerLabel;
	
	//Check if actor exists, and add if not
	bsActor* actor = nullptr;
	for (auto nextActor : actors) {
		if (nextActor->actor == source) {
			actor = nextActor;
			break;
		}
	}
	if (!actor) {
		actor = new bsActor;
		actor->actor = source;
		actors.push_back(actor);
	}

	//Now that we have an actor, add the performance to the queue
	actor->performances.push(performance);
}


void BattleScene::addAction(bMonster* source, bsType type, AnimType action, SDL_Point target, std::string bYell) {
	bsPerform* performance = new bsPerform;
	performance->actor = source;
	performance->type = type;
	performance->animation = action;
	performance->targetPoint = target;
	if (type == bsMoveTo) performance->direction = getDirection(source->pos, target);
	else performance->direction = source->direction;
	performance->battleYell = bYell;

	//Check if actor exists, and add if not
	bsActor* actor = nullptr;
	for (auto nextActor : actors) {
		if (nextActor->actor == source) {
			actor = nextActor;
			break;
		}
	}
	if (!actor) {
		actor = new bsActor;
		actor->actor = source;
		actors.push_back(actor);
	}

	//Now that we have an actor, add the performance to the queue
	actor->performances.push(performance);
}


void BattleScene::start() {
	started = true;
}


bool BattleScene::isStarted() {
	return started;
}


bool BattleScene::isFinished() {
	return finished;
}


void BattleScene::step() {
	if (!started || finished) return;

	for (auto actor : actors) perform(actor);

	while (newActors.size() > 0) {
		bsActor* newActor = newActors.front();
		actors.push_back(newActor);
		newActors.erase(newActors.begin());
	}

	std::vector<bsActor*>::iterator itr;
	for(itr = actors.begin(); itr != actors.end();)
	{
		bsActor* actor = *itr;
		if(actor->performances.size() == 0)
		{
			itr = actors.erase(itr);
		}
		else ++itr;
	}
	if (actors.size() == 0) {
		finished = true;
		updateAudience(true);
	}
}

void BattleScene::perform(bsActor* actor) {
	
	bsPerform* perform = actor->performances.front();
	bool performanceComplete = false;

	switch (perform->type) {
	case bsMoveTo:
		performanceComplete = perform_moveTo(perform);
		break;
	case bsFadeTo:
		performanceComplete = perform_fadeTo(perform);
		break;
	case bsAction:
		performanceComplete = perform_action(perform);
		break;
	case bsStandby:
		performanceComplete = perform_standBy(perform);
		break;
	case bsReaction:
		performanceComplete = perform_reaction(perform);
		break;
	case bsFaint:
		performanceComplete = perform_faint(perform);
		break;
		
	case bsCaptureBegin:
		performanceComplete = perform_captureBegin(perform);
		break;
	case bsCaptureDrag:
		performanceComplete = perform_captureDrag(perform);
		break;
	case bsCaptureSuccess:
		performanceComplete = perform_captureSuccess(perform);
		break;
	case bsCaptureAngry:
		performanceComplete = perform_captureAngry(perform);
		break;
	case bsCaptureFail:
		performanceComplete = perform_captureFail(perform);
		break;
	case bsVictory:
		performanceComplete = perform_victory(perform);
		break;
	}

	if (performanceComplete) {
		actor->performances.pop();
		delete perform;
	}
}

bool BattleScene::perform_moveTo(bsPerform* perform) {
	bMonster* actor = perform->actor;
	SDL_Point target = perform->targetPoint;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = AnimType::Walk;
		actor->direction = getDirection(actor->pos, target);
		actor->setAnimation(actor->current_animation, actor->direction);
	}

	float dtMove = 20.0;
	float distance = sqrtf( powf((target.x - actor->pos.x),2) + powf((target.y - actor->pos.y), 2));
	float dtRem = distance - dtMove;

	if (actor->pos.x != target.x || actor->pos.y != target.y) {
		if (dtMove < distance) {
			actor->pos.x = ((dtMove * target.x) + (dtRem * actor->pos.x)) / distance;
			actor->pos.y = ((dtMove * target.y) + (dtRem * actor->pos.y)) / distance;
		}
		else {
			actor->pos.x = target.x;
			actor->pos.y = target.y;
		}

		actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
	}
	else return true;

	return false;
}


bool BattleScene::perform_fadeTo(bsPerform* perform) {
	bMonster* actor = perform->actor;
	SDL_Point target = perform->targetPoint;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		perform->startPoint = { actor->pos.x, actor->pos.y };
		actor->current_animation = AnimType::Walk;
		actor->direction = getDirection(actor->pos, target);
		actor->setAnimation(actor->current_animation, actor->direction);
	}

	float dtMove = 10.0;
	float origDistance = sqrtf(powf((target.x - perform->startPoint.x), 2) + powf((target.y - perform->startPoint.y), 2));
	float distance = sqrtf(powf((target.x - actor->pos.x), 2) + powf((target.y - actor->pos.y), 2));
	
	float falpha = (distance / origDistance) * 255.0;
	BYTE alpha = (BYTE)falpha;
	if (actor->secEffect == beSphere) actor->secEffectSprite->setAlpha(alpha);
	else actor->animation[actor->current_animation_name]->setAlpha(alpha);
	
	float dtRem = distance - dtMove;
	if (actor->pos.x != target.x || actor->pos.y != target.y) {
		if (dtMove < distance) {
			actor->pos.x = ((dtMove * target.x) + (dtRem * actor->pos.x)) / distance;
			actor->pos.y = ((dtMove * target.y) + (dtRem * actor->pos.y)) / distance;
		}
		else {
			actor->pos.x = target.x;
			actor->pos.y = target.y;
		}

		actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
	}
	else {
		if (actor->secEffect == beSphere) actor->secEffectSprite->setAlpha(0xFF);
		else actor->animation[actor->current_animation_name]->setAlpha(0xFF);
		return true;
	}

	return false;
}


bool BattleScene::perform_action(bsPerform* perform) {
	bMonster* actor = perform->actor;
	SDL_Point target = perform->targetPoint;
	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = perform->animation;
		if (actor->type == bActorType::ENEMY) actor->direction = default_monster_dir;
		else actor->direction = default_ally_dir;
		actor->setAnimation(actor->current_animation, actor->direction);
		actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
		actor->animation[actor->current_animation_name]->repeatMode = 1;
		actor->animation[actor->current_animation_name]->start();
		
		if (perform->triggerLabel) floatingLabels->push_back(perform->triggerLabel);

		startReaction();

		lookTo = target;
		updateAudience();
	}

	if (actor->animation[actor->current_animation_name]->repeatCount >= 1) return true;

	return false;
}


bool BattleScene::perform_standBy(bsPerform* perform) {
	bMonster* actor = perform->actor;
	actor->current_animation = AnimType::StandBy;
	if (actor->type == bActorType::ENEMY) actor->direction = default_monster_dir;
	else actor->direction = default_ally_dir;
	actor->setAnimation(actor->current_animation, actor->direction);
	return true;
}


bool BattleScene::perform_reaction(bsPerform* perform) {
	bMonster* actor = perform->actor;
	
	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = perform->animation;
		actor->setAnimation(actor->current_animation, actor->direction);
	}

	int elapsedTime = SDL_GetTicks() - perform->startTime;
	if (elapsedTime >= perform->playTime) return true;

	return false;
}


bool BattleScene::perform_faint(bsPerform* perform) {
	bMonster* actor = perform->actor;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = perform->animation;
		actor->setAnimation(actor->current_animation, actor->direction);
		actor->animation[actor->current_animation_name]->start();
		actor->alive = false;
	}

	int elapsedTime = SDL_GetTicks() - perform->startTime;
	if (actor->animation[actor->current_animation_name]->repeatCount >= 1) {
		actor->current_animation = AnimType::Lie;
		actor->setAnimation(actor->current_animation, actor->direction);
		//actor->effect = beFaint;
		actor->setEffect(beFaint);
		//actor->secEffect = beNone;
		return true;
	}

	return false;
}


bool BattleScene::perform_captureBegin(bsPerform* perform) {
	bMonster* player = reactor;
	bMonster* actor = perform->actor;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = AnimType::StandBy;

		actor->direction = 1;
		actor->setAnimation(actor->current_animation, actor->direction);
		//actor->effect = beMirror;
		actor->setEffect(beMirror);

		player->current_animation = AnimType::Cast;
		player->setAnimation(player->current_animation, player->direction);
		player->animation[player->current_animation_name]->repeatMode = 2;
		player->animation[player->current_animation_name]->start();
	}

	int currentTicks = (SDL_GetTicks() - perform->startTime);
	int currentSecs = currentTicks / 1000;

	//spin in a circle with mirror above
	if (currentSecs < 1) {
		int intervalSize = 1000 / 24; //two rotations
		int newDir = default_monster_dir + (currentTicks / intervalSize);
		while (newDir > 7) newDir -= 8;
		if (actor->direction != newDir) {
			actor->direction = newDir;
			actor->setAnimation(actor->current_animation, actor->direction);
		}
	}
	else return true;

	return false;
}


bool BattleScene::perform_captureDrag(bsPerform* perform) {
	bMonster* player = reactor;
	bMonster* actor = perform->actor;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = AnimType::StandBy;

		actor->direction = 1;
		actor->setAnimation(actor->current_animation, actor->direction);
		//actor->effect = beMirror;
		actor->setEffect(beMirror);

		player->current_animation = AnimType::Cast;
		player->setAnimation(player->current_animation, player->direction);
		player->animation[player->current_animation_name]->repeatMode = 2;
		player->animation[player->current_animation_name]->start();
	}

	int currentTicks = (SDL_GetTicks() - perform->startTime);
	int currentSecs = currentTicks / 1000;

	//Drag the sphere to mid
	if (currentSecs == 0) {
		//actor->secEffect = beSphere;
		actor->setSecondaryEffect(beSphere);

		//actor->secEffectSprite->setAlpha(0xFF);
		actor->animation[actor->current_animation_name]->visible = false;

		SDL_Point target;
		target.x = ((player->basePos.x - actor->basePos.x) / 2) + actor->basePos.x;
		target.y = ((player->basePos.y - actor->basePos.y) / 2) + actor->basePos.y;
		float distance = sqrtf(powf((target.x - actor->pos.x), 2) + powf((target.y - actor->pos.y), 2));

		float dtMove = 5.0;
		float speed = 6.0;
		float dtRem = distance - dtMove;

		if (actor->pos.x != target.x || actor->pos.y != target.y) {
			//Move the orb to the middle
			if (dtMove < distance) {
				//actor->pos.x = (int)(((dtMove * target.x) + (dtRem * actor->pos.x)) / distance + 0.5);
				//actor->pos.y = (int)(((dtMove * target.y) + (dtRem * actor->pos.y)) / distance + 0.5);

				float dx = target.x - actor->pos.x;
				float dy = target.y - actor->pos.y;
				float length = sqrtf(dx*dx + dy*dy);
				dx /= length;
				dy /= length;
				dx *= speed;
				dy *= speed;
				actor->pos.x += (int)(dx + 0.5);
				actor->pos.y += (int)(dy + 0.5);
			}
			else {
				actor->pos.x = target.x;
				actor->pos.y = target.y;
			}

			actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
		}
	}

	//Vibrate to resist capture
	if (currentSecs == 1) {
		SDL_Point midPoint;
		midPoint.x = ((player->basePos.x - actor->basePos.x) / 2) + actor->basePos.x;
		midPoint.y = ((player->basePos.y - actor->basePos.y) / 2) + actor->basePos.y;
		float midPoint_distance = sqrtf(powf((player->basePos.x - midPoint.x), 2) + powf((player->basePos.y - midPoint.y), 2));
		float distance = sqrtf(powf((player->basePos.x - actor->pos.x), 2) + powf((player->basePos.y - actor->pos.y), 2));

		float maxMove = 30.0;

		float dtMove = (rand() % 3) + 1.0;
		int moveDir = rand() % 2;
		if (moveDir == 1) dtMove = dtMove;
		else dtMove = -dtMove;

		float dtRem = distance - dtMove;
		actor->pos.x = ((dtMove * player->basePos.x) + (dtRem * midPoint.x)) / distance;
		actor->pos.y = ((dtMove * player->basePos.y) + (dtRem * midPoint.y)) / distance;
		actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
	}

	//
	if (currentSecs == 3) {
		return true;
	}

	return false;
}


bool BattleScene::perform_captureSuccess(bsPerform* perform) {
	bMonster* player = reactor;
	bMonster* actor = perform->actor;
	SDL_Point target = perform->targetPoint;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->alive = false;
		//actor->effect = beNone;
		actor->setEffect(beNone);
	}	

	SDL_Point midPoint;
	midPoint.x = ((player->basePos.x - actor->basePos.x) / 2) + actor->basePos.x;
	midPoint.y = ((player->basePos.y - actor->basePos.y) / 2) + actor->basePos.y;
	float midPointDistance = sqrtf(powf((target.x - midPoint.x), 2) + powf((target.y - midPoint.y), 2));

	float dtMove = 2.5;
	float speed = 6.0;
	float distance = sqrtf(powf((target.x - actor->pos.x), 2) + powf((target.y - actor->pos.y), 2));
	float dtRem = distance - dtMove;

	float alphaPerc = distance / midPointDistance;
	BYTE alpha = (BYTE)((float)alphaPerc * 255.0);
	actor->secEffectSprite->setAlpha(alpha);

	if (actor->pos.x != target.x || actor->pos.y != target.y) {
		if (dtMove < distance) {
			//actor->pos.x = (int)(((dtMove * target.x) + (dtRem * actor->pos.x)) / distance + 0.5);
			//actor->pos.y = (int)(((dtMove * target.y) + (dtRem * actor->pos.y)) / distance + 0.5);

			float dx = target.x - actor->pos.x;
			float dy = target.y - actor->pos.y;
			float length = sqrtf(dx*dx + dy*dy);
			dx /= length;
			dy /= length;
			dx *= speed;
			dy *= speed;
			actor->pos.x += (int)(dx + 0.5);
			actor->pos.y += (int)(dy + 0.5);
		}
		else {
			actor->pos.x = target.x;
			actor->pos.y = target.y;
		}

		actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
	}
	else {
		//actor->secEffect = beNone;
		actor->setSecondaryEffect(beNone);
		player->setAnimation(AnimType::StandBy, player->direction);
		return true;
	}

	return false;
}


bool BattleScene::perform_captureAngry(bsPerform* perform) {
	bMonster* player = reactor;
	bMonster* actor = perform->actor;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = perform->animation;
		actor->setAnimation(actor->current_animation, actor->direction);
		actor->animation[actor->current_animation_name]->start();
		actor->animation[actor->current_animation_name]->visible = true;
		//actor->secEffect = beNone;
		actor->setSecondaryEffect(beNone);
		player->setAnimation(AnimType::StandBy, player->direction);
	}

	int currentTicks = (SDL_GetTicks() - perform->startTime);
	int currentSecs = currentTicks / 1000;

	//Spin in reverse
	if (currentSecs == 0) {
		int intervalSize = 1000 / 24; //two rotations
		int newDir = default_monster_dir - (currentTicks / intervalSize);
		while (newDir < 0) newDir += 8;
		if (actor->direction != newDir) {
			actor->direction = newDir;
			actor->setAnimation(actor->current_animation, actor->direction);
			actor->animation[actor->current_animation_name]->setLocation(actor->pos.x, actor->pos.y);
			actor->animation[actor->current_animation_name]->visible = true;
		}
	}

	if (currentSecs == 1) {
		//actor->effect = beNone;
		actor->setEffect(beNone);
		actor->battleYell = perform->battleYell;
		actor->direction = default_monster_dir;
	}

	if (currentSecs == 2) {
		actor->battleYell = "";
		player->current_animation = AnimType::StandBy;
		player->setAnimation(player->current_animation, player->direction);
		return true;
	}

	return false;
}


bool BattleScene::perform_captureFail(bsPerform* perform) {
	bMonster* actor = perform->actor;
	SDL_Point target = perform->targetPoint;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = AnimType::Walk;
		actor->direction = getDirection(actor->pos, target);
		actor->setAnimation(actor->current_animation, actor->direction);
		actor->animation[actor->current_animation_name]->visible = true;
		//actor->effect = beNone;
		//actor->secEffect = beNone;
		actor->setEffect(beNone);
		actor->setSecondaryEffect(beNone);
	}

	float dtMove = 20.0;
	float distance = sqrtf(powf((target.x - actor->pos.x), 2) + powf((target.y - actor->pos.y), 2));
	float dtRem = distance - dtMove;

	if (actor->pos.x != target.x || actor->pos.y != target.y) {
		if (dtMove < distance) {
			actor->pos.x = ((dtMove * target.x) + (dtRem * actor->pos.x)) / distance;
			actor->pos.y = ((dtMove * target.y) + (dtRem * actor->pos.y)) / distance;
		}
		else {
			actor->pos.x = target.x;
			actor->pos.y = target.y;
		}
	}
	else return true;

	return false;
}


bool BattleScene::perform_victory(bsPerform* perform) {
	bMonster* player = reactor;
	bMonster* actor = perform->actor;

	if (!perform->started) {
		perform->started = true;
		perform->startTime = SDL_GetTicks();
		actor->current_animation = perform->animation;
		actor->setAnimation(actor->current_animation, actor->direction);
		actor->animation[actor->current_animation_name]->start();
		actor->animation[actor->current_animation_name]->visible = true;
		//actor->secEffect = beNone;
		actor->setSecondaryEffect(beNone);
	}

	int currentTicks = (SDL_GetTicks() - perform->startTime);
	int currentSecs = currentTicks / 1000;

	if (currentSecs < 2) {
		actor->battleYell = perform->battleYell;
		actor->direction = default_ally_dir;
	}
	else {
		actor->battleYell = "";
		return true;
	}

	return false;
}


void BattleScene::startReaction() {
	bsPerform* performance = getReactionPerformance();
	if (performance) {
		if (performance->type == bsReaction) {
			performance->startTime = SDL_GetTicks();
			return;
		}
	}

	bsActor* performActor = new bsActor;
	performActor->actor = reactor;

	bsPerform *performReaction = new bsPerform;

	if (reaction != AnimType::Faint) {
		performReaction->actor = reactor;
		performReaction->animation = reaction;
		performReaction->playTime = 1000;
		performReaction->type = bsReaction;
		performActor->performances.push(performReaction);

		performReaction = new bsPerform;
		performReaction->actor = reactor;
		performReaction->type = bsStandby;
		performActor->performances.push(performReaction);
	}
	else {
		performReaction->actor = reactor;
		performReaction->animation = AnimType::Wound;
		performReaction->playTime = 1000;
		performReaction->type = bsReaction;
		performActor->performances.push(performReaction);

		performReaction = new bsPerform;
		performReaction->actor = reactor;
		performReaction->animation = AnimType::Faint;
		performReaction->type = bsFaint;
		performActor->performances.push(performReaction);
	}

	newActors.push_back(performActor);
}


bsPerform* BattleScene::getReactionPerformance() {
	bsActor* reactionActor = nullptr;
	for (auto actor : actors) {
		if (actor->actor == reactor) {
			reactionActor = actor;
			break;
		}
	}
	if (!reactionActor) return nullptr;

	bsPerform* performance = reactionActor->performances.front();
	return performance;
}


void BattleScene::updateAudience(bool reset) {
	if (!reset) {
		//The audience watches the actors
		for (auto actor : cast) {
			if (actor == reactor) continue;
			if (!actor->alive) continue;

			bool isAudience = true;
			for (auto anActor : actors) {
				if (anActor->actor == actor) {
					isAudience = false;
					break;
				}
			}

			if (isAudience) {
				actor->direction = getDirection(actor->pos, lookTo);
				actor->setAnimation(actor->current_animation, actor->direction);
			}
		}
	}
	else {
		//reset to default look directions
		for (auto actor : cast) {
			if (actor->type == bActorType::ENEMY) actor->direction = default_monster_dir;
			else actor->direction = default_ally_dir;
			actor->setAnimation(actor->current_animation, actor->direction);
		}
	}
}