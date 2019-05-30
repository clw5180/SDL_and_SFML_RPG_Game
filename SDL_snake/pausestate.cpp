#include "pausestate.h"
#include "MainMenuState.h"
//#include "PlayState.h"
#include "TextureManager.h"
#include "Game.h"
#include "MenuButton.h"
#include "InputHandler.h"
#include "StateParser.h"

const std::string PauseState::s_pauseID = "PAUSE";

void PauseState::s_pauseToMain()
{
	TheGame::Instance()->getStateMachine()->changeState(new MainMenuState());
}

void PauseState::s_resumePlay()
{
	TheGame::Instance()->getStateMachine()->popState();
}

void PauseState::update()
{
	if (/*m_loadingComplete &&*/ !m_gameObjects.empty())  //clw note：m_loadingComplete暂时还没加
	{
		for (unsigned int i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i]->update();
		}
	}
}
void PauseState::render()
{
	if (/*m_loadingComplete &&*/ !m_gameObjects.empty())
	{
		for (unsigned int i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i]->draw();
		}
	}
}
bool PauseState::onEnter()
{
	StateParser stateParser;
	stateParser.parseState("test.xml", s_pauseID, &m_gameObjects,
		&m_textureIDList);
	m_callbacks.push_back(0);
	m_callbacks.push_back(s_pauseToMain);
	m_callbacks.push_back(s_resumePlay);

	setCallbacks(m_callbacks); 
	//clw note：如果没有上面一句，回调函数的函数指针为0，比如menubutton中m_callback != 0，无法调用回调函数，因此返回主界面失败

	m_loadingComplete = true; //clw note：暂时没用到，后面需要再加

	std::cout << "entering PauseState\n";
	return true;


	/************************************************************************/
	/*              clw note：在没用xml解析器之前，是这样实现的               */
	/************************************************************************/
	//if (!TheTextureManager::Instance()->load("assets/resume.png", "resumebutton", TheGame::Instance()->getRenderer()))
	//{
	//	return false;
	//}
	//if (!TheTextureManager::Instance()->load("assets/main.png", "mainbutton", TheGame::Instance()->getRenderer()))
	//{
	//	return false;
	//}

	////GameObject* button1 = new MenuButton(new LoaderParams(200, 100, 200, 80, "mainbutton"), s_pauseToMain);
	////GameObject* button2 = new MenuButton(new LoaderParams(200, 300, 200, 80, "resumebutton"), s_resumePlay);

	//GameObject* button1 = new MenuButton();
	//GameObject* button2 = new MenuButton();

	//m_gameObjects.push_back(button1);
	//m_gameObjects.push_back(button2);
	//std::cout << "entering PauseState\n";
	//return true;
}

bool PauseState::onExit()
{
	if (/*m_loadingComplete &&*/ !m_gameObjects.empty())
	{
		// clear the texture manager
		for (vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i]->clean();
			delete m_gameObjects[i];
		}
		m_gameObjects.clear();
	}

	// clear the texture manager
	for (vector<string>::size_type i = 0; i < m_textureIDList.size(); i++)
	{
		TheTextureManager::Instance()->clearFromTextureMap(m_textureIDList[i]);
	}
	TheInputHandler::Instance()->reset();

	std::cout << "exiting PauseState\n";
	return true;


	//for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	//{
	//	m_gameObjects[i]->clean();
	//}
	//m_gameObjects.clear();

	//TheTextureManager::Instance()->clearFromTextureMap("resumebutton");
	//TheTextureManager::Instance()->clearFromTextureMap("mainbutton");

	//// reset the mouse button states to false
	//TheInputHandler::Instance()->reset();
	//std::cout << "exiting PauseState\n";
	//return true;
}

void PauseState::setCallbacks(const std::vector<Callback>& callbacks)
{
	// go through the game objects
	if (!m_gameObjects.empty())
	{
		for (vector<GameObject*>::size_type i = 0; i < m_gameObjects.size(); i++)
		{
			// if they are of type MenuButton then assign a callback based on the id passed in from the file
			if (dynamic_cast<MenuButton*>(m_gameObjects[i]))
			{
				MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
				pButton->setCallback(callbacks[pButton->getCallbackID()]);
			}
		}
	}
}