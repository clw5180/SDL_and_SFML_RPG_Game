#ifndef __PLAYSTATE__
#define __PLAYSTATE__

#include "gamestate.h"
#include <vector>
//#include "level.h"

class GameObject;

class PlayState : public GameState
{
public:
	
	virtual ~PlayState(){}

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_playID; }

private:
	static const std::string s_playID;

	std::vector<GameObject*> m_gameObjects;

	//Level* pLevel;
};

#endif

