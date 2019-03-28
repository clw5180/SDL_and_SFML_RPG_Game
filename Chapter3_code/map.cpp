#include "map.h"
#include "inputhandler.h" 
#include "global.h"

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
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) /*&& m_position.GetY() > 0*/)
	{
		m_y += 4;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) /*&& (m_position.GetY() + m_height) < CGame::Instance()->GetWindowHeight() - 10*/)
	{
		m_y -= 4;
	}
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) /*&& m_position.GetX() > 0*/)
	{
		m_x += 4;
	}
	else if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) /*&& (m_position.GetX() + m_width) < CGame::Instance()->GetWindowWidth()*/)
	{
		m_x -= 4;
	}

	//clw note：for debug，仅用于打印调试信息
	if (CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) ||
		CInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		std::cout << "x_Map:" << m_x << "  ";  //clw note：for debug
        std::cout << "y_Map:" << m_y << std::endl;
		std::cout << "x_Role:" << (SCREEN_WIDTH - 56) / 2 - m_x << "  ";   //TODO：一帧人物的宽高；去掉hard code
		std::cout << "y_Role:" << (SCREEN_HEIGHT - 91) / 2 - m_y << std::endl;
	}
}

void CMap::Render()
{
	SDL_Rect srcRect = { 0, 0, MAP_WIDTH, MAP_HEIGHT }; //TODO，新建一个纹理类CTexture，在LoadImage()的时候把图片读入列表
										 //      并记录图片尺寸，代替这里的MAP_WIDTH和MAP_HEIGHT

//角色在地图靠左位置时，地图靠窗口左边显示；角色在地图靠右位置时，地图靠窗口右边显示；
//上下方向同理。
	int tempX = CMap::Instance()->GetX();
	int tempY = CMap::Instance()->GetY();
	if (tempX > 0)
		tempX = 0;
	if (tempX < -(MAP_WIDTH - SCREEN_WIDTH))
		tempX = -(MAP_WIDTH - SCREEN_WIDTH);
	if (tempY > 0)
		tempY = 0;
	if (tempY < -(MAP_HEIGHT - SCREEN_HEIGHT))
		tempY = -(MAP_HEIGHT - SCREEN_HEIGHT);
	SDL_Rect destRect = { tempX, tempY, MAP_WIDTH, MAP_HEIGHT }; //TODO：替换这里的2560和1200
	SDL_RenderCopy(g_pRenderer, g_pMapTexture, &srcRect, &destRect); //将纹理复制到渲染器中；
}