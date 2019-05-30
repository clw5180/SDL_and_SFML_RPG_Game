//==============================================================================
/*
	贪吃蛇 made by clw 
	20180818 ~ 20180821
	还有几个地方可以进一步修改：
	☆☆☆1、增加fps，修改参数SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	       看是否有变化 —— 这里只设置fps不行，必须设置一个行进速度velocity，
	       帧率如果设成默认的一秒60帧，相当于16.6ms就会过一次，如果一次移动32个像素点，
		   那么1s会移动32*60像素，太多了，但是不想减小帧率，也不能减小每次移动的像素值；
		   因此考虑加入一个count，比如count==30 / velocity的时候才进行Update，
		   然后count清零；否则count++；如果velocity=1，那么相当于1s会count60次，
		   相当于1s移动64个像素点。

	☆☆☆2、增加得分显示，吃一个食物+10分，需要进一步学习ttf
	☆   3、食物位置虽然是随机出现的，但不应该出现在蛇身或蛇头
	☆☆ 4、增加窗口栏左侧的小图标
	☆☆ 5、增加tile于边界处
	☆   6、比如现在是向右行进，如果按左键此时不应该判断为碰撞自身从而结束游戏


	已解决 1——通过设置速度，然后在Update内计次m_count为60/velocity；（这样还是会在Update卡住，有没有更好的办法？多线程？）
	已解决 2；
	已解决 6——在Update内加入相应的判断即可；

	20180913
	可以把控制台打出来方便调试 ———— 项目属性-链接-系统-控制台

	20180918
	↑↓←→事件处理还需优化！！！！目前是存一个当前按下键值的截图，也就是可能同时按下两个键；但是之前是用switch(e.key.keysym.sym)，只有一条路可走；
	另外可以考虑用Vector2D类型的速度变量，通过正负和x、y来判断上下左右；
	考虑多线程；
*/
//==============================================================================
#include "Game.h"

const int FPS = 60;
const float DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* argv[]) 
{
	Uint32 frameStart, frameTime;

	if (!Game::Instance()->Init())
		return -1;

	while (Game::Instance()->running())
	{
		frameStart = SDL_GetTicks();
		
		Game::Instance()->HandleEvents();
		Game::Instance()->Update();
		Game::Instance()->Render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}

	Game::Instance()->CleanUp();

	return 0;
}
