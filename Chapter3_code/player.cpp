#include "player.h"
#include "inputhandler.h"
#include "global.h"  //for SCREEN_WIDTH，SCREEN_HEIGHT
#include "map.h"     //处理角色行走到地图边界的情形


int g_moveVelocity = 4; //clw note：debug

CPlayer* CPlayer::s_pInstance = NULL;

CPlayer::CPlayer()
{
	m_x = (SCREEN_WIDTH - m_w) / 2;
	m_y = (SCREEN_HEIGHT - m_h) / 2;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
	/*
	  根据角色位置的不同，角色在地图上的移动可分为两大类：
	  （1）角色在地图中间时：让角色显示在屏幕中间位置，即
	  m_x = (SCREEN_WIDTH - m_w) / 2;
	  m_y = (SCREEN_HEIGHT - m_h) / 2;
	  （2）角色在地图边缘位置（靠上/下/左/右）
	  在检测到角色位于地图边缘后，应保证地图不动，角色离开窗口中心位置；
	  处理较为复杂，见下。
	                                                                  */
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LSHIFT))  //按下左侧shift后开启跑动模式
	{ 
		g_moveVelocity = 5;  //跑动速度
		m_bWalkState = false;
	}
	else
	{ 
		g_moveVelocity = 3;  //行走速度
		m_bWalkState = true;
	}

	//依次处理左、右、上、下四种按键
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT)) //向左走
	{
		m_bWalkState ? m_currentRow = 4 : m_currentRow = 8;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERLEFT;
		//角色在地图靠左或靠右的位置，向左走
		m_x -= g_moveVelocity; //如在地图靠左侧区域：角色离开窗口中心位置继续向左移动；如在地图靠右侧区域：角色从右边向中心点位置返回。
		if (CMap::Instance()->GetX() >= 0) //左侧边缘区域  TODO：这个大于等于改成等于？？
		{	
			if (m_x < 0)   //防止在左侧区域时，角色向左移动到地图边缘之外
				m_x = 0;
		}
		else if (CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH)) //右侧边缘区域
		{
			if (m_x < (SCREEN_WIDTH - m_w) / 2)   //防止角色向左移动超过窗口中心点
				m_x = (SCREEN_WIDTH - m_w) / 2;
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT)) //向右走
	{
		m_bWalkState ? m_currentRow = 2 : m_currentRow = 6;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHT;
		m_x += g_moveVelocity; //如在地图靠左侧区域：角色从左边向中心点位置返回。如在地图靠右侧区域：角色离开窗口中心位置继续向右移动；
		if (CMap::Instance()->GetX() >= 0) //左侧边缘区域
		{
			if (m_x > (SCREEN_WIDTH - m_w) / 2)  //防止角色向右移动超过窗口中心点
				m_x = (SCREEN_WIDTH - m_w) / 2; 
		}
		else if (CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH))//右侧边缘区域
		{
			if (m_x > SCREEN_WIDTH - m_w)   //防止在右侧区域时，角色向右移动到地图边缘之外
				m_x = SCREEN_WIDTH - m_w;
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}

	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP)) //向上走
	{
		m_bWalkState ? m_currentRow = 3 : m_currentRow = 7;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERUP;
		m_y -= g_moveVelocity;
		if (CMap::Instance()->GetY() >= 0)   //上边缘区域
		{
			if (m_y < 0)    //防止角色向上移动到地图边缘之外
				m_y = 0;
		}
		else if (CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT)) //下边缘区域
		{
			if (m_y < (SCREEN_HEIGHT - m_h) / 2)  //防止向上走超过窗口中心点
				m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
		else
		{
			m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN)) //向下走
	{
		m_bWalkState ? m_currentRow = 1 : m_currentRow = 5;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERDOWN;
		m_y += g_moveVelocity;
		if (CMap::Instance()->GetY() >= 0)  //上边缘区域
		{
			if (m_y > (SCREEN_HEIGHT - m_h) / 2)
				m_y = (SCREEN_HEIGHT - m_h) / 2; //防止向下走超过窗口中心点
		}
		else if (CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT))  //下边缘区域
		{
			if (m_y > SCREEN_HEIGHT - m_h)   //防止角色向下移动到地图边缘之外
				m_y = SCREEN_HEIGHT - m_h;
		}
		else
		{
			m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
	}

	//依次处理左下，右上，左上，右下
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))
	{
		m_bWalkState ? m_currentRow = 4 : m_currentRow = 8;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTDOWN;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP))
	{
		m_bWalkState ? m_currentRow = 2 : m_currentRow = 6;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHTUP;
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		m_bWalkState ? m_currentRow = 3 : m_currentRow = 7;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTUP;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		m_bWalkState ? m_currentRow = 1 : m_currentRow = 5;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHTDOWN;
	}

	//上下左右都没按下，就面向之前移动的方向静止
	if (!CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT)
		&& !CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT)
		&& !CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP)
		&& !CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))
	{
		m_currentRow = 0;
		switch (m_movedirection)
		{
		case PLAYERLEFT:
			m_currentFrame = 6;
			break;
		case PLAYERRIGHT:
			m_currentFrame = 2;
			break;
		case PLAYERUP:
			m_currentFrame = 4;
			break;
		case PLAYERDOWN:
			m_currentFrame = 0;
			break;
		case PLAYERLEFTDOWN:
			m_currentFrame = 7;
			break;
		case PLAYERRIGHTUP:
			m_currentFrame = 3;
			break;
		case PLAYERLEFTUP:
			m_currentFrame = 5;
			break;
		case PLAYERRIGHTDOWN:
			m_currentFrame = 1;
			break;
		default:
			break;
		}
	}
}

void CPlayer::Render()
{
	//Render current frame
    //size of SpriteSheet: 448 * 819 = (56*8) * (91*9)        
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = m_w * m_currentFrame;
	srcRect.y = m_h * m_currentRow;
	srcRect.w = destRect.w = m_w;
	srcRect.h = destRect.h = m_h;

	destRect.x = m_x;
	destRect.y = m_y;

	SDL_RenderCopy(g_pRenderer, g_pSpriteSheetTexture, &srcRect, &destRect);
}