#include "camera.h"
#include "inputhandler.h" 
#include "global.h"

CCamera* CCamera::s_pInstance = NULL;

CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

//=========================================================
//函数名称：Update
//说明：在game.cpp中的Update函数中被调用，根据键盘↑↓←→等
//      不同按键，更新相机坐标
//=========================================================
void CCamera::Update()
{
	//handle keys
	//注意：这里角色向下走，相当于相机在往上走；
	//     相机在人的上面；
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) /*&& m_position.getY() > 0*/)
	{
		m_y += 3;
		if (m_y > 0)
			m_y = 0;
	}
	else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) /*&& (m_position.getY() + m_height) < CGame::Instance()->GetWindowHeight() - 10*/)
	{
		m_y -= 3;
		if (m_y < -SCREEN_HEIGHT)
			m_y = -SCREEN_HEIGHT;
	}
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) /*&& m_position.getX() > 0*/)
	{
		m_x += 3;
		if(m_x > 0)
			m_x = 0;
	}
	else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) /*&& (m_position.getX() + m_width) < CGame::Instance()->GetWindowWidth()*/)
	{
		m_x -= 3;
		if (m_x < -SCREEN_WIDTH)
	    	m_x = -SCREEN_WIDTH;
	}
}