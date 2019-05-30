#ifndef __GAMESTATEMACHINE__
#define __GAMESTATEMACHINE__

#include <vector>
#include "gamestate.h"

class GameStateMachine
{
public:
	GameStateMachine() {}
	~GameStateMachine() {}

	void update();
	void render();

	void pushState(GameState* pState);
	void changeState(GameState* pState);
	void popState();

private:
	std::vector<GameState*> m_gameStates;
};

#endif