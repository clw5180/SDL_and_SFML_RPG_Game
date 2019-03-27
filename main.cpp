#include "game.h"
#include <SDL.h>

//设置游戏帧率（FPS）
const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

//=========================================================
//函数名称：main
//说明：主函数
//=========================================================
int main(int argc, char* argv[]) //注意这里必须在main的括号内加上int argc, char* argv[]否则报错"LNK2019无法解析的外部符号 _SDL_main"
{
	Uint32 frameStart, frameTime; 

	if (!CGame::Instance()->Init())  //Step1：初始化SDL
	{
		printf("Failed to Init!\n");
		return -1;
	}

	while (CGame::Instance()->Running())
	{
		frameStart = SDL_GetTicks();

		
		CGame::Instance()->HandleEvents();  //Step2：事件处理
		CGame::Instance()->Update();  //Step3：更新状态
		CGame::Instance()->Render();  //Step4：渲染，绘制图像

		frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}

	CGame::Instance()->Close();  //Step5：结束程序，释放内存
	return 0;
}