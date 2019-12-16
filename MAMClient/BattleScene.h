#ifndef __BATTLESCENE_H
#define __BATTLESCENE_H

#include <SDL.h>
#include <vector>
#include <queue>
#include <string>

class bMonster;
class bAlly;
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
	bsVictory
};

class Texture;
struct FloatingLabel {
	~FloatingLabel() {
		if (top) delete top;
		if (bottom) delete bottom;
	}
	Texture* top = NULL;
	Texture* bottom = NULL;
	SDL_Point target;
	SDL_Point position;
	bool started = false;
	int startTime;
};

struct bsPerform {
	bMonster* actor;
	int animation;
	int direction;
	SDL_Point targetPoint;
	SDL_Point startPoint;
	bsType type;
	FloatingLabel *triggerLabel;
	std::string battleYell;

	bool started = false;
	int startTime;
	int playTime;
};

struct bsActor {
	bMonster* actor;
	std::queue<bsPerform*> performances;
};

class BattleScene {
private:
	std::vector<bMonster*> cast;
	std::vector<bsActor*> actors, newActors;
	bMonster* reactor;
	int reaction;
	SDL_Point lookTo;

	const int default_ally_dir = 5;
	const int default_monster_dir = 1;

	bool started = false, finished = false;
	std::vector<FloatingLabel*> *floatingLabels;

public:
	BattleScene(std::vector<bAlly*> allies, std::vector<bMonster*> monsters, std::vector<FloatingLabel*> *fLabels);
	~BattleScene();

	void BattleScene::setReactor(bMonster* target, int action);
	void BattleScene::addAction(bMonster* source, bsType type, AnimType action, SDL_Point target, FloatingLabel *triggerLabel = nullptr);
	void BattleScene::addAction(bMonster* source, bsType type, AnimType action, SDL_Point target, std::string battleYell);

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
	bool BattleScene::perform_victory(bsPerform* perform);

	void BattleScene::startReaction();
	bsPerform* BattleScene::getReactionPerformance();
	void BattleScene::updateAudience(bool reset = false);
};

#endif