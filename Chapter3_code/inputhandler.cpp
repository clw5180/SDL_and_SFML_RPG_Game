#include <iostream>
#include "inputhandler.h"
#include "game.h"  //主要用于接收到SDL_QUIT类型的事件时（如点击窗口右上角退出按钮），
                   //则退出游戏主循环；因此需要和CGame类交互。

CInputHandler* CInputHandler::s_pInstance = 0;

CInputHandler::CInputHandler() :  m_keystates(0),
m_bJoysticksInitialised(false),
m_mousePosition(new Vector2D(0,0))
{
    // create button states for the mouse
    for(int i = 0; i < 3; i++)
    {
        m_mouseButtonStates.push_back(false);
    }
}

CInputHandler::~CInputHandler()
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

void CInputHandler::clean()
{
    // we need to clean up after ourselves and close the joysticks we opened
    if(m_bJoysticksInitialised)
    {
        for(unsigned int i = 0; i < SDL_NumJoysticks(); i++)
        {
            SDL_JoystickClose(m_joysticks[i]);
        }
    }
}

void CInputHandler::initialiseJoysticks()
{
    // if we haven't already initialised the joystick subystem, we will do it here
    if(SDL_WasInit(SDL_INIT_JOYSTICK) == 0)
    {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    }
    
    // get the number of joysticks, skip init if there aren't any
    if(SDL_NumJoysticks() > 0)
    {
        for(int i = 0; i < SDL_NumJoysticks(); i++)
        {
            // create a new joystick
            SDL_Joystick* joy = SDL_JoystickOpen(i);
            
            // if the joystick opened correctly we need to populate our arrays
            if(SDL_JoystickOpen(i))
            {
                // push back into the array to be closed later
                m_joysticks.push_back(joy);
                
                // create a pair of values for the axes, a vector for each stick
                m_joystickValues.push_back(std::make_pair(new Vector2D(0,0),new Vector2D(0,0)));
                
                // create an array to hold the button values
                std::vector<bool> tempButtons;
                
                // fill the array with a false value for each button
                for(int j = 0; j <  SDL_JoystickNumButtons(joy); j++)
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

void CInputHandler::reset()
{
    m_mouseButtonStates[LEFT] = false;
    m_mouseButtonStates[RIGHT] = false;
    m_mouseButtonStates[MIDDLE] = false;
}

bool CInputHandler::isKeyDown(SDL_Scancode key) const
{
    if(m_keystates != NULL)
    {
        if(m_keystates[key] == 1)
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

int CInputHandler::getAxisX(int joy, int stick) const
{
    if(m_joystickValues.size() > 0)
    {
        if(stick == 1)
        {
            return m_joystickValues[joy].first->getX();
        }
        else if(stick == 2)
        {
            return m_joystickValues[joy].second->getX();
        }
    }
    return 0;
}

int CInputHandler::getAxisY(int joy, int stick) const
{
    if(m_joystickValues.size() > 0)
    {
        if(stick == 1)
        {
            return m_joystickValues[joy].first->getY();
        }
        else if(stick == 2)
        {
            return m_joystickValues[joy].second->getY();
        }
    }
    return 0;
}

bool CInputHandler::getButtonState(int joy, int buttonNumber) const
{
    return m_buttonStates[joy][buttonNumber];
}

bool CInputHandler::getMouseButtonState(int buttonNumber) const
{
    return m_mouseButtonStates[buttonNumber];
}

Vector2D* CInputHandler::getMousePosition() const
{
    return m_mousePosition;
}

void CInputHandler::update()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
				CGame::Instance()->Quit();
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
                break;
                
            case SDL_KEYUP:
                onKeyUp();
                break;
                
            default:
                break;
        }
    }
}

void CInputHandler::onKeyDown()
{
    m_keystates = SDL_GetKeyboardState(0);
}

void CInputHandler::onKeyUp()
{
    m_keystates = SDL_GetKeyboardState(0);
}

//2、鼠标部分
void CInputHandler::onMouseMove(SDL_Event &event)
{
    m_mousePosition->setX(event.motion.x);
    m_mousePosition->setY(event.motion.y);
	//std::cout << "x_mouse:" << event.motion.x << "  ";   //clw note：for debug, detect mouse position 
	//std::cout << "y_mouse:" << event.motion.y << std::endl;
}

void CInputHandler::onMouseButtonDown(SDL_Event &event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        m_mouseButtonStates[LEFT] = true;
    }
    
    if(event.button.button == SDL_BUTTON_MIDDLE)
    {
        m_mouseButtonStates[MIDDLE] = true;
    }
    
    if(event.button.button == SDL_BUTTON_RIGHT)
    {
        m_mouseButtonStates[RIGHT] = true;
    }
}

void CInputHandler::onMouseButtonUp(SDL_Event &event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        m_mouseButtonStates[LEFT] = false;
    }
    
    if(event.button.button == SDL_BUTTON_MIDDLE)
    {
        m_mouseButtonStates[MIDDLE] = false;
    }
    
    if(event.button.button == SDL_BUTTON_RIGHT)
    {
        m_mouseButtonStates[RIGHT] = false;
    }
}

//3、手柄部分
void CInputHandler::onJoystickAxisMove(SDL_Event &event)
{
    int whichOne = event.jaxis.which;
    
	// clw note：我的北通手柄具体按键配置和一般手柄不同，因为只有左边一个摇杆。
	// event.jaxis.axis == 0、1、2、3，用来控制摇杆的左右按键，
    // event.jaxis.axis == 4，         用来控制摇杆的上下按键。
	// 而一般都和这里一样有两个摇杆，比如xbox360的手柄
	// 左摇杆是控制角色移动的，右摇杆是控制视角的，对于3D游戏来说是必要的
	// 用0轴控制左摇杆的左右，1轴控制左摇杆的上下，
	//   4轴控制右摇杆的上下，3轴控制右摇杆的左右。

    // left stick move left or right        
    if(event.jaxis.axis == 0)      
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
    
    //left stick move up or down   
	//if (event.jaxis.axis == 1) //我的北通手柄和例子不一样，event.jaxis.axis == 1实际控制左右按键而不是上下
	if (event.jaxis.axis == 4)   //clw modify
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
	/*********************************************************************
	*   经过测试，北通BTP-1126型手柄可用轴数为5                 
	*   其中0,1,2,3表示x轴移动，当在Debug模式下按下手柄十字键的→键， 
	*   move_axis0~3变量值为32767，即代表向右移动，而move_axis4的值为-256  
	*   按下手柄十字键的↓键时，move_axis0~3的值为-256,而move_axis4的值为32767
	*   说明北通BTP-1126型手柄x轴由axis0~3均可表示，y轴由axis4表示，很另类
	*   SDL官方的wiki对于SDL_JoystickGetAxis函数下面给出了一个Remarks： 
	*   对于多数现代的摇杆，左右摇杆的x轴通常由axis=0表示，y轴通常由axis=1表示      
	**********************************************************************/
	int AvailableAxesNum = SDL_JoystickNumAxes(m_joysticks.back());

	Sint16 move_axis0;
	Sint16 move_axis1;
	Sint16 move_axis2;
	Sint16 move_axis3;
	Sint16 move_axis4;
	
	move_axis0 = SDL_JoystickGetAxis(m_joysticks.back(), 0);
	move_axis1 = SDL_JoystickGetAxis(m_joysticks.back(), 1);
	move_axis2 = SDL_JoystickGetAxis(m_joysticks.back(), 2);
	move_axis3 = SDL_JoystickGetAxis(m_joysticks.back(), 3);
	move_axis4 = SDL_JoystickGetAxis(m_joysticks.back(), 4);
	/*********************************************************************/

	//clw note：没有右摇杆，因此这里屏蔽掉。

    // right stick move left or right  
    //if(event.jaxis.axis == 3)
    //{
    //    if (event.jaxis.value > m_joystickDeadZone)
    //    {
    //        m_joystickValues[whichOne].second->setX(1);
    //    }
    //    else if(event.jaxis.value < -m_joystickDeadZone)
    //    {
    //        m_joystickValues[whichOne].second->setX(-1);
    //    }
    //    else
    //    {
    //        m_joystickValues[whichOne].second->setX(0);
    //    }
    //}
    
    // right stick move up or down
    //if(event.jaxis.axis == 4)
    //{
		//if (event.jaxis.value > m_joystickDeadZone)
		//{
		//	m_joystickValues[whichOne].second->setY(1);
		//}
		//else if (event.jaxis.value < -m_joystickDeadZone)
		//{
		//	m_joystickValues[whichOne].second->setY(-1);
		//}
		//else
		//{
		//	m_joystickValues[whichOne].second->setY(0);
		//}
    //}
}

void CInputHandler::onJoystickButtonDown(SDL_Event &event)
{
    int whichOne = event.jaxis.which;
    
    m_buttonStates[whichOne][event.jbutton.button] = true;
}

void CInputHandler::onJoystickButtonUp(SDL_Event &event)
{
    int whichOne = event.jaxis.which;
    
    m_buttonStates[whichOne][event.jbutton.button] = false;
}