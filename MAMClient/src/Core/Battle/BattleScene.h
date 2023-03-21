#ifndef __BATTLESCENE_H
#define __BATTLESCENE_H

#include <SDL.h>
#include <vector>
#include <queue>
#include <string>

class Fighter;
enum AnimType;

const int default_ally_dir = 4;
const int default_enemy_dir = 0;

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

	Fighter* actor;
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
	Fighter* actor;
	std::queue<bsPerform*> performances;
};

class BattleScene {
private:
	std::vector<Fighter*> cast;
	std::vector<bsActor*> actors, newActors;
	Fighter* reactor;
	int reaction;
	SDL_Point lookTo;

	bool started = false, finished = false;

public:
	BattleScene(std::vector<Fighter*> allies, std::vector<Fighter*> enemies);
	~BattleScene();

	void BattleScene::setReactor(Fighter* target, int action);
	bsPerform* BattleScene::addAction(Fighter* source, bsType type, AnimType action, SDL_Point target, std::vector<std::string> floatingLabels);
	bsPerform* BattleScene::addAction(Fighter* source, bsType type, AnimType action, SDL_Point target);

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