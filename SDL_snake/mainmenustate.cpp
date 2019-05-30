#include "mainmenustate.h"
#include "TextureManager.h"
#include "game.h"
#include "gamestate.h"
#include "menubutton.h"
#include "PlayState.h"
#include "InputHandler.h"
#include <vector>
#include "StateParser.h"

const std::string MainMenuState::s_menuID = "MENU";

//Callbacks
void MainMenuState::s_menuToPlay()
{
	std::cout << "Play button clicked\n"; //测试用

	TheGame::Instance()->getStateMachine()->changeState(new PlayState());
}

void MainMenuState::s_exitFromMenu()
{
	std::cout << "Exit button clicked\n";  //测试用

	TheGame::Instance()->quit();
}

void MainMenuState::update()
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->update();
	}
}

void MainMenuState::render()
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->draw();
	}
}

bool MainMenuState::onEnter()
{
	// parse the state
	StateParser stateParser;
	stateParser.parseState("test.xml", s_menuID, &m_gameObjects,
		&m_textureIDList);			//clw note：这里解析xml文件，会把所有东西都加载进来
	m_callbacks.push_back(0);      //pushback 0 callbackID start from 1
	m_callbacks.push_back(s_menuToPlay);
	m_callbacks.push_back(s_exitFromMenu);
	// set the callbacks for menu items
	setCallbacks(m_callbacks);
	std::cout << "entering MenuState\n";
	return true;


	/************************************************************************/
	/*              clw note：在没用xml解析器之前，是这样实现的               */
	/************************************************************************/
	////std::cout << "entering MenuState\n"; //测试用
	//if (!TheTextureManager::Instance()->load("assets/button.png",
	//	"playbutton", TheGame::Instance()->getRenderer()))
	//{
	//	return false;
	//}
	//if (!TheTextureManager::Instance()->load("assets/exit.png",
	//	"exitbutton", TheGame::Instance()->getRenderer()))
	//{
	//	return false;
	//}

	////GameObject* button1 = new MenuButton(new LoaderParams(120, 100, 400, 100, "playbutton"), s_menuToPlay);
	////GameObject* button2 = new MenuButton(new LoaderParams(120, 280, 400, 100, "exitbutton"), s_exitFromMenu);
	//GameObject* button1 = new MenuButton();
	//GameObject* button2 = new MenuButton();
	//m_gameObjects.push_back(button1);
	//m_gameObjects.push_back(button2);
	//std::cout << "entering MenuState\n";
	//return true;
}

bool MainMenuState::onExit()
{
	m_exiting = true;

	// clean the game objects
	if (/*m_loadingComplete &&*/ !m_gameObjects.empty())
	{
		m_gameObjects.back()->clean();
		m_gameObjects.pop_back();
	}

	// clear the texture manager
	for(vector<string>::size_type i = 0; i < m_textureIDList.size(); i++)
	{
	TheTextureManager::Instance()->clearFromTextureMap(m_textureIDList[i]);
	}
	

	m_gameObjects.clear();

	// reset the input handler
	TheInputHandler::Instance()->reset();

	std::cout << "exiting MenuState\n";
	return true;


	/************************************************************************/
	/*              clw note：在没用xml解析器之前，是这样实现的               */
	/************************************************************************/
	////std::cout << "exiting MenuState\n"; //测试用
	//for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	//{
	//	m_gameObjects[i]->clean();
	//}
	//m_gameObjects.clear();
	//TheTextureManager::Instance()
	//	->clearFromTextureMap("playbutton");
	//TheTextureManager::Instance()
	//	->clearFromTextureMap("exitbutton");
	//std::cout << "exiting MenuState\n";
	//return true;


}

void MainMenuState::setCallbacks(const std::vector<Callback>& callbacks)
{
	if (!m_gameObjects.empty()) //后来加的
	{
		// go through the game objects
		for (vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
		{
			// if they are of type MenuButton then assign a callback based on the id passed in from the file
			if (dynamic_cast<MenuButton*>(m_gameObjects[i]))
			{
				MenuButton* pButton =
					dynamic_cast<MenuButton*>(m_gameObjects[i]);
				pButton->setCallback(callbacks[pButton->getCallbackID()]);
			}
		}
	}
}