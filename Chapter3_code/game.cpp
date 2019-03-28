#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>  //这里直接使用SDL_image库，可以加载如.jpg .png等各种格式的图片
#include "game.h"
#include "inputhandler.h"
#include "global.h"
#include "map.h"
#include "player.h"

SDL_Window *g_pWindow = NULL;
SDL_Renderer *g_pRenderer = NULL;
SDL_Texture *g_pMapTexture = NULL;
SDL_Texture *g_pSpriteSheetTexture = NULL;

// 窗口宽高
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// 地图宽高
const int MAP_WIDTH = 2560;
const int MAP_HEIGHT = 1200;

// 静态成员变量的初始化
CGame* CGame::s_pInstance = NULL;

// 构造函数
CGame::CGame()
{
}

// 析构函数
CGame::~CGame()
{
}

//=========================================================
//函数名称：LoadImage
//说明：加载图片到纹理对象
//=========================================================
bool LoadImage(const char* filename, SDL_Texture*& texture)
{
	if (filename == NULL)
	{
		printf("LoadImage() error: filename is NULL!\n");
		return false;
	}

	SDL_Surface *pTempSurface = IMG_Load(filename);  //加载启动图像(splash image)到SDL_Surface对象
	if (pTempSurface == NULL)
	{
		printf("Unable to load image and recommend to check the path of image file! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	texture = SDL_CreateTextureFromSurface(g_pRenderer, pTempSurface); //使用SDL_Surface对象创建纹理(texture)
	if (texture == NULL)
	{
		printf("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_FreeSurface(pTempSurface);  //回收surface对象占用的内存空间
	return true;
}


//=========================================================
//函数名称：Init
//说明：完成SDL初始化、创建窗口等
//=========================================================
bool CGame::Init()  //初始化
{
	//（1）初始化所有SDL的各模块
	if (SDL_Init(SDL_INIT_VIDEO) < 0) //这里可以初始化SDL的各种模块，包括定时器，视频，音频，摇杆等
									  //实际项目中一般只初始化要用到的模块即可，具体有哪些模块可以通过
									  //在SDL_INIT_VIDEO上点击右键-转到定义来查看；成功返回0
	{
		printf("Unable to Init SDL: %s", SDL_GetError()); //调用SDL_GetError()来查看失败原因
		return false;
	}

	//（2）使用SDL创建窗口
	g_pWindow = SDL_CreateWindow("Chapter 1: Hello World!",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (g_pWindow == NULL)
	{
		printf("Unable to Create Window: %s", SDL_GetError());
		return false;
	}

	//（3）初始化渲染器(renderer)，用于在窗口绘图
	g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
	if (g_pRenderer == NULL)
	{
		printf("Unable to Create Renderer: %s", SDL_GetError());
		return false;
	}

	//（4）加载图片
	if (!LoadImage("./res/town.png", g_pMapTexture))
	{
		printf("Failed to load image 'town.png'!\n");
		return false;
	}
	if (!LoadImage("./res/xiahouyi.png", g_pSpriteSheetTexture))
	{
		printf("Failed to load image 'xiahouyi.png'!\n");
		return false;
	}
	return true;
}

//=========================================================
//函数名称：HandleEvents
//说明：输入设备事件处理，包括鼠标、键盘、手柄等
//=========================================================
void CGame::HandleEvents() 
{
	CInputHandler::Instance()->update();
}


//=========================================================
//函数名称：Update
//说明：根据不同的事件对游戏状态或内容进行更新
//=========================================================
void CGame::Update() 
{
	CPlayer::Instance()->Update();
	CMap::Instance()->Update();
}


//=========================================================
//函数名称：Render
//说明：渲染，完成图像的绘制
//=========================================================
void CGame::Render()  
{
	SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 255, 0);  //设置渲染器的颜色，第一个参数传入渲染器对象
             												//接下来依次是R、G、B值和Alpha值
    //绘制图片三板斧:SDL_RenderClear()、SDL_RenderCopy()、SDL_RenderPresent()
	SDL_RenderClear(g_pRenderer);  //使用某种绘图颜色来“清除”当前窗口

	CMap::Instance()->Render();
	CPlayer::Instance()->Render();

	SDL_RenderPresent(g_pRenderer);  //将渲染器中的内容在窗口中显示出来
}


//=========================================================
//函数名称：Close
//说明：销毁各对象
//=========================================================
void CGame::Close()
{
	//销毁窗口对象
	SDL_DestroyWindow(g_pWindow);
	g_pWindow = NULL;  //当g_pWindow不再指向窗口时，手动设为NULL；

	//销毁渲染器对象
	SDL_DestroyRenderer(g_pRenderer);
	g_pRenderer = NULL;

	//销毁纹理对象
	SDL_DestroyTexture(g_pMapTexture);
	g_pMapTexture = NULL;
	SDL_DestroyTexture(g_pSpriteSheetTexture);
	g_pSpriteSheetTexture = NULL;

	//清除所有已初始化的子系统，并退出SDL。在程序结束时调用。
	SDL_Quit();  
}