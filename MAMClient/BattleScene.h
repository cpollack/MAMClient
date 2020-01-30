#ifndef __BATTLESCENE_H
#define __BATTLESCENE_H

#include <SDL.h>
#include <vector>
#include <queue>
#include <string>

class Entity;
enum AnimType;

enum bsType {
	bsMoveTo,
	bsFadeTo,
	bsStandby,
	bsAction,
	bsReaction,
	bsFaint,
	bsCaptureBegin,
	bsCaptureDrag,
	bsCaptureSuccess,
	bsCaptureAngry,
	bsCaptureFail,
	bsUseItem,
	bsVictory
};

class Texture;

struct bsPerform {
	bsPerform() { SDL_zero(boundEvent); }

	Entity* actor;
	int animation;
	int effect = 0;
	int direction;
	SDL_Point targetPoint;
	SDL_Point startPoint;
	bsType type;
	std::vector<std::string> floatingLabels;
	std::string battleYell;

	bool started = false;
	int startTime;
	int playTime;
	SDL_Event boundEvent;
};

struct bsActor {
	Entity* actor;
	std::queue<bsPerform*> performances;
};

class BattleScene {
private:
	std::vector<Entity*> cast;
	std::vector<bsActor*> actors, newActors;
	Entity* reactor;
	int reaction;
	SDL_Point lookTo;

	const int default_ally_dir = 5;
	const int default_monster_dir = 1;

	bool started = false, finished = false;

public:
	BattleScene(std::vector<Entity*> allies, std::vector<Entity*> enemies);
	~BattleScene();

	void BattleScene::setReactor(Entity* target, int action);
	bsPerform* BattleScene::addAction(Entity* source, bsType type, AnimType action, SDL_Point target, std::vector<std::string> floatingLabels);
	bsPerform* BattleScene::addAction(Entity* source, bsType type, AnimType action, SDL_Point target);

	void BattleScene::start();
	bool BattleScene::isStarted();
	bool BattleScene::isFinished();

	void BattleScene::step();
	void BattleScene::perform(bsActor* actor);
	bool BattleScene::perform_moveTo(bsPerform* perform);
	bool BattleScene::perform_fadeTo(bsPerform* perform);
	bool BattleScene::perform_action(bsPerform* perform);
	bool BattleScene::perform_standBy(bsPerform* perform);
	bool BattleScene::perform_reaction(bsPerform* perform);
	bool BattleScene::perform_faint(bsPerform* perform);
	bool BattleScene::perform_captureBegin(bsPerform* perform);
	bool BattleScene::perform_captureDrag(bsPerform* perform);
	bool BattleScene::perform_captureSuccess(bsPerform* perform);
	bool BattleScene::perform_captureAngry(bsPerform* perform);
	bool BattleScene::perform_captureFail(bsPerform* perform);
	bool BattleScene::perform_useItem(bsPerform* perform);
	bool BattleScene::perform_victory(bsPerform* perform);

	void BattleScene::startReaction();
	bsPerform* BattleScene::getReactionPerformance();
	void BattleScene::updateAudience(bool reset = false);
};

#endif