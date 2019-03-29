#include "map.h"
#include "inputhandler.h" 
#include "global.h"
#include "player.h"

CMap* CMap::s_pInstance = NULL;

CMap::CMap()
{
}

CMap::~CMap()
{
}

//=========================================================
//函数名称：Update
//说明：在game.cpp中的Update函数中被调用，根据键盘↑↓←→等
//      不同按键，更新Map坐标
//=========================================================
void CMap::Update()
{
	//handle keys
	//注意：这里角色向上走，可以看做角色不动，而screen image向下移动；
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) && m_y < 0 
		&& CPlayer::Instance()->GetY() == (SCREEN_HEIGHT - CPlayer::Instance()->GetHeight()) / 2)
	{
		m_y += g_moveVelocity;
		if (m_y > 0)
			m_y = 0; //clw note：地图位置修正，不要超出边界
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) && m_y > -(MAP_HEIGHT - SCREEN_HEIGHT)
		&& CPlayer::Instance()->GetY() == (SCREEN_HEIGHT - CPlayer::Instance()->GetHeight()) / 2)
	{
		m_y -= g_moveVelocity;
		if (m_y < -(MAP_HEIGHT - SCREEN_HEIGHT)) 
			m_y = -(MAP_HEIGHT - SCREEN_HEIGHT);
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) && m_x < 0 
		&& CPlayer::Instance()->GetX() == (SCREEN_WIDTH - CPlayer::Instance()->GetWidth()) / 2)
	{
		m_x += g_moveVelocity;
		if (m_x > 0)
			m_x = 0;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) && m_x > -(MAP_WIDTH - SCREEN_WIDTH) 
		&& CPlayer::Instance()->GetX() == (SCREEN_WIDTH - CPlayer::Instance()->GetWidth()) / 2) //角色如果在地图靠左的位置，必须向右走到窗口中心点才能移动地图
	{
		m_x -= g_moveVelocity;
		if (m_x < -(MAP_WIDTH - SCREEN_WIDTH))
			m_x = -(MAP_WIDTH - SCREEN_WIDTH);
	}

	//clw note：for debug，仅用于打印调试信息
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		std::cout << "x_Map:" << m_x << "  ";  //clw note：for debug
        std::cout << "y_Map:" << m_y << std::endl;
	}
}

void CMap::Render()
{
	SDL_Rect srcRect = { 0, 0, MAP_WIDTH, MAP_HEIGHT }; //TODO，新建一个纹理类CTexture，在LoadImage()的时候把图片读入列表
										 //      并记录图片尺寸，代替这里的MAP_WIDTH和MAP_HEIGHT
	SDL_Rect destRect = { m_x, m_y, MAP_WIDTH, MAP_HEIGHT }; //TODO：替换这里的2560和1200
	SDL_RenderCopy(g_pRenderer, g_pMapTexture, &srcRect, &destRect); //将纹理复制到渲染器中；
}