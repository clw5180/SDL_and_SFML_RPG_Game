#include "playstate.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "Game.h"
#include "InputHandler.h"
//#include "LevelParser.h"
//#include "Level.h"
#include "snake.h"
#include "TextureManager.h"
#include "StateParser.h"
#include "SoundManager.h"

const std::string PlayState::s_playID = "PLAY";


void PlayState::update()
{
	//pLevel->update();
	//clw note ?
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->getStateMachine()->pushState(new PauseState());
	}
	
	if (TheGame::Instance()->GetGameOverFlag())
	{
		TheGame::Instance()->getStateMachine()->changeState(new GameOverState());
	}
	
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->update();
	}
}

void PlayState::render()
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->draw();
	}
	
}

bool PlayState::onEnter()
{
	/***************************************************************/
	//clw modify 20180918 进入游戏后播放游戏背景音乐
	SDL_Delay(1000);
	SoundManager::Instance()->playMusic("BackgroundMusic", -1);

	TheGame::Instance()->SetGameOverFlag(false);
	/***************************************************************/

	// parse the state
	StateParser stateParser;
	//clw note：这里的解析，相当于load各种object对象！
	stateParser.parseState("test.xml", s_playID, &m_gameObjects, &m_textureIDList);  

	//LevelParser levelParser;
	//pLevel = levelParser.parseLevel("assets/map1.tmx");

	std::cout << "entering PlayState\n";

	return true;
	

	/************************************************************************/
	/*              clw note：在没用xml解析器之前，是这样实现的               */
	/************************************************************************/
	//std::cout << "entering PlayState\n";  //测试用

	//if (!TheTextureManager::Instance()->load("assets/helicopter.png", "helicopter", TheGame::Instance()->getRenderer()))
	//{
	//	return false;
	//}

	////GameObject* player = new Player(new LoaderParams(100, 100, 128, 55, "helicopter"));
	//GameObject* player = new Player();
	//m_gameObjects.push_back(player);
	//return true;
}

bool PlayState::onExit()
{
	std::cout << "exiting PlayState\n";  //测试用

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->clean();
	}
	m_gameObjects.clear();
	//TheTextureManager::Instance()->clearFromTextureMap("helicopter");

	// add by clw 20180918
	Mix_HaltMusic();

	return true;
}