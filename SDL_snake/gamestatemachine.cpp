#include "gamestatemachine.h"


void GameStateMachine::pushState(GameState *pState)
{
	m_gameStates.push_back(pState);
	m_gameStates.back()->onEnter();
}

void GameStateMachine::popState()
{
	if (!m_gameStates.empty())
	{
		if (m_gameStates.back()->onExit())
		{
			delete m_gameStates.back();
			m_gameStates.pop_back();
		}
	}
}

//《SDL Game Development》原程序
void GameStateMachine::changeState(GameState *pState)
{
	if (!m_gameStates.empty())
	{
		if (m_gameStates.back()->getStateID() == pState->getStateID())
		{
			return; // do nothing
		}
		m_gameStates.back()->onExit();
		//delete m_gameStates.back();  //加上后狂点Play和Main Menu会卡死？？？
		m_gameStates.pop_back();
	}

	// initialise it
	pState->onEnter();

	// push back our new state
	m_gameStates.push_back(pState);
}

void GameStateMachine::update()
{
	if (!m_gameStates.empty())
	{
		m_gameStates.back()->update();  //we use back() to get the current state;
										//this is because we have designed our FSM to always use the state at the back of the array
	}
}

void GameStateMachine::render()
{
	if (!m_gameStates.empty())
	{
		m_gameStates.back()->render();
	}
}