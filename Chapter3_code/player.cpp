#include "player.h"
#include "inputhandler.h"
#include "global.h"  //for SCREEN_WIDTH，SCREEN_HEIGHT
#include "map.h"     //处理角色行走到地图边界的情形

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
	//根据角色位置的不同，角色在地图上的移动可分为3种情况：
	//（1）正常角色都会在地图中间：让角色显示在屏幕中间位置，即
	//m_x = (SCREEN_WIDTH - m_w) / 2;
	//m_y = (SCREEN_HEIGHT - m_h) / 2;

	//依次处理左、右、上、下
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		m_currentRow = 4;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERLEFT;
		//（2）角色在地图靠左或靠右的位置时
		if (CMap::Instance()->GetX() >= 0 || CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH))
		{
			m_x -= 4; // 允许角色继续向左移动；
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		m_currentRow = 2;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHT;
		if (CMap::Instance()->GetX() <= -(MAP_WIDTH - SCREEN_WIDTH) || CMap::Instance()->GetX() >= 0) //角色在地图靠右位置时
		{
			m_x += 4;  //允许角色继续向右移动，同理。
		}
		else
		{
			m_x = (SCREEN_WIDTH - m_w) / 2;
		}
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP))
	{
		m_currentRow = 3;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERUP;
		//（3）角色在地图靠上或靠下的位置，允许角色继续向上或向下移动，同理。
		if (CMap::Instance()->GetY() >= 0 || CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT))
		{
			m_y -= 4;
		}
		else
		{
			m_y = (SCREEN_HEIGHT - m_h) / 2;
		}
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))
	{
		m_currentRow = 1;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2)));
		m_movedirection = PLAYERDOWN;
		if (CMap::Instance()->GetY() <= -(MAP_HEIGHT - SCREEN_HEIGHT) || CMap::Instance()->GetY() >= 0)
		{
			m_y += 4;
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
		m_currentRow = 4;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTDOWN;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP))
	{
		m_currentRow = 2;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERRIGHTUP;
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		m_currentRow = 3;
		m_currentFrame = int(((SDL_GetTicks() / (150)) % (m_numFrames / 2) + (m_numFrames / 2)));
		m_movedirection = PLAYERLEFTUP;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) &&
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		m_currentRow = 1;
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