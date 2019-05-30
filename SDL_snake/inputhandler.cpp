//
//  InputHandler.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 24/01/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#include "inputhandler.h"
#include "Game.h"

InputHandler* InputHandler::s_pInstance = NULL; //特别注意：static成员必须在类外初始化！另外不必再加static关键字

InputHandler::InputHandler() : m_keystates(0), 
m_bJoysticksInitialised(false),
m_mousePosition(new Vector2D(0, 0))   //一定要注意m_mousePosition指针的初始化！
{
	// create button states for the mouse
	for (int i = 0; i < 3; i++)
	{
		m_mouseButtonStates.push_back(false);//初始化m_mouseButtonStates，非常重要！！
	}
}

InputHandler::~InputHandler()
{
	// delete anything we created dynamically
	delete m_keystates;
	delete m_mousePosition;

	// clear our arrays
	m_joystickValues.clear();
	m_joysticks.clear();
	m_buttonStates.clear();
	m_mouseButtonStates.clear();
}

void InputHandler::clean()
{
	// we need to clean up after ourselves and close the joysticks we opened
	if (m_bJoysticksInitialised)
	{
		for (int i = 0; i < SDL_NumJoysticks(); i++)
		{
			SDL_JoystickClose(m_joysticks[i]);
		}
	}
}

void InputHandler::initialiseJoysticks()
{
	// if we haven't already initialised the joystick subystem, we will do it here
	if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0)
	{
		SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	}

	// get the number of joysticks, skip init if there aren't any
	if (SDL_NumJoysticks() > 0)
	{
		for (int i = 0; i < SDL_NumJoysticks(); i++)
		{
			// create a new joystick
			SDL_Joystick* joy = SDL_JoystickOpen(i);

			// if the joystick opened correctly we need to populate our arrays
			if (SDL_JoystickOpen(i))
			{
				// push back into the array to be closed later
				m_joysticks.push_back(joy);

				// create a pair of values for the axes, a vector for each stick
				m_joystickValues.push_back(std::make_pair(new Vector2D(0, 0), new Vector2D(0, 0)));

				// create an array to hold the button values
				std::vector<bool> tempButtons;

				// fill the array with a false value for each button
				for (int j = 0; j < SDL_JoystickNumButtons(joy); j++)
				{
					tempButtons.push_back(false);
				}
				// push the button array into the button state array
				m_buttonStates.push_back(tempButtons);
			}
			else
			{
				// if there was an error initialising a joystick we want to know about it
				std::cout << SDL_GetError();
			}
		}

		// enable joystick events
		SDL_JoystickEventState(SDL_ENABLE);
		m_bJoysticksInitialised = true;

		std::cout << "Initialised " << m_joysticks.size() << " joystick(s)";
	}
	else
	{
		m_bJoysticksInitialised = false;
	}
}

void InputHandler::reset()
{
	m_mouseButtonStates[LEFT] = false;
	m_mouseButtonStates[RIGHT] = false;
	m_mouseButtonStates[MIDDLE] = false;
}

bool InputHandler::isKeyDown(SDL_Scancode key) const 
{
	if (m_keystates != NULL)
	{
		if (m_keystates[key] == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

float InputHandler::getAxisX(int joy, int stick) const
{
	if (m_joystickValues.size() > 0)
	{
		if (stick == 1)
		{
			return m_joystickValues[joy].first->getX();
		}
		else if (stick == 2)
		{
			return m_joystickValues[joy].second->getX();
		}
	}
	return 0;
}

float InputHandler::getAxisY(int joy, int stick) const
{
	if (m_joystickValues.size() > 0)
	{
		if (stick == 1)
		{
			return m_joystickValues[joy].first->getY();
		}
		else if (stick == 2)
		{
			return m_joystickValues[joy].second->getY();
		}
	}
	return 0;
}

bool InputHandler::getMouseButtonState(int buttonNumber) const
{
	return m_mouseButtonStates[buttonNumber];
}

Vector2D* InputHandler::getMousePosition() const
{
	return m_mousePosition;
}

void InputHandler::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;

		case SDL_JOYAXISMOTION:
			onJoystickAxisMove(event);
			break;

		case SDL_JOYBUTTONDOWN:
			onJoystickButtonDown(event);
			break;

		case SDL_JOYBUTTONUP:
			onJoystickButtonUp(event);
			break;

		case SDL_MOUSEMOTION:
			onMouseMove(event);
			break;

		case SDL_MOUSEBUTTONDOWN:
			onMouseButtonDown(event);
			break;

		case SDL_MOUSEBUTTONUP:
			onMouseButtonUp(event);
			break;

		case SDL_KEYDOWN:
			onKeyDown();
			//clw modify 20180916
			//switch (event.key.keysym.sym)
			//{
			//case SDLK_UP:
			
			if (event.key.repeat == 1)
			{
				if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) +
					TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) +
					TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) +
					TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) >= 2)
					m_bKeyRepeat = false;
				else
					m_bKeyRepeat = true;
			}
			else
				m_bKeyRepeat = false;
			break;

		case SDL_KEYUP:
			onKeyUp();
			break;

		default:
			break;
		}
	}
}

void InputHandler::onKeyDown()
{
	m_keystates = SDL_GetKeyboardState(0);  
	//函数功能：拍一个当前键盘各种按键的快照，可能有的键按下了，有的没按下；用一个keystate array表示。
	//如果括号内参数非0，则返回keystate array长度；如果为0，则返回keystate array的指针，之后可如下操作：
	/*if (m_keystates[SDL_SCANCODE_RETURN]) 
	 *{
	 *	printf("<RETURN> is pressed.\n");
	 *}
	 */
#if _DEBUG
	if (m_keystates[SDL_SCANCODE_UP])
	{
		printf("<UP> is pressed.  ");
	}
	if (m_keystates[SDL_SCANCODE_DOWN])
	{
		printf("<DOWN> is pressed.  ");
	}
	if (m_keystates[SDL_SCANCODE_LEFT])
	{
		printf("<LEFT> is pressed.  ");
	}
	if (m_keystates[SDL_SCANCODE_RIGHT])
	{
		printf("<RIGHT> is pressed.  ");
	}
	printf("\n");
#endif
}

void InputHandler::onKeyUp()
{
	m_keystates = SDL_GetKeyboardState(0);
}

void InputHandler::onMouseMove(SDL_Event &event)
{
	m_mousePosition->setX((float)event.motion.x);
	m_mousePosition->setY((float)event.motion.y);
}

void InputHandler::onMouseButtonDown(SDL_Event &event)
{
	if (event.button.button == SDL_BUTTON_LEFT)
	{
		m_mouseButtonStates[LEFT] = true;
	}

	if (event.button.button == SDL_BUTTON_MIDDLE)
	{
		m_mouseButtonStates[MIDDLE] = true;
	}

	if (event.button.button == SDL_BUTTON_RIGHT)
	{
		m_mouseButtonStates[RIGHT] = true;
	}
}

void InputHandler::onMouseButtonUp(SDL_Event &event)
{
	if (event.button.button == SDL_BUTTON_LEFT)
	{
		m_mouseButtonStates[LEFT] = false;
	}

	if (event.button.button == SDL_BUTTON_MIDDLE)
	{
		m_mouseButtonStates[MIDDLE] = false;
	}

	if (event.button.button == SDL_BUTTON_RIGHT)
	{
		m_mouseButtonStates[RIGHT] = false;
	}
}

void InputHandler::onJoystickAxisMove(SDL_Event &event)
{
	int whichOne = event.jaxis.which;

	// left stick move left or right
	if (event.jaxis.axis == 0)
	{
		if (event.jaxis.value > m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setX(1);
		}
		else if (event.jaxis.value < -m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setX(-1);
		}
		else
		{
			m_joystickValues[whichOne].first->setX(0);
		}
	}

	// left stick move up or down
	if (event.jaxis.axis == 4)
	{
		if (event.jaxis.value > m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setY(1);
		}
		else if (event.jaxis.value < -m_joystickDeadZone)
		{
			m_joystickValues[whichOne].first->setY(-1);
		}
		else
		{
			m_joystickValues[whichOne].first->setY(0);
		}
	}
}

void InputHandler::onJoystickButtonDown(SDL_Event &event)
{
	int whichOne = event.jaxis.which;

	m_buttonStates[whichOne][event.jbutton.button] = true;
}

void InputHandler::onJoystickButtonUp(SDL_Event &event)
{
	int whichOne = event.jaxis.which;

	m_buttonStates[whichOne][event.jbutton.button] = false;
}



