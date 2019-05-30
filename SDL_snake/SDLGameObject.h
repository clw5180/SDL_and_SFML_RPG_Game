#ifndef __SDLGAMEOBJECT_H__
#define __SDLGAMEOBJECT_H__

#include <SDL.h>
#include "gameobject.h"

using namespace std;

class SDLGameObject : public GameObject
{
public:
	
	virtual ~SDLGameObject() {}

	virtual void draw();
	virtual void update();
	virtual void clean();
	virtual void load(std::unique_ptr<LoaderParams> const &pParams);

	virtual std::string type() { return "SDLGameObject"; }

protected:
	SDLGameObject();


};

#endif