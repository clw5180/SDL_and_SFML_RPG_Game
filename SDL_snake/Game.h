//文件名称：Game
//作者：夜雨潇湘
//说明：Game类
//日期：2018-08

#ifndef __GAME_H__
#define __GAME_H__

#include <SDL.h>
#include "gamestatemachine.h"

class Game
{
public:
	static Game* Instance()
	{
		if (pInstance == NULL)
		{
			pInstance = new Game();
		}
		return pInstance;
	}

	// Initialize our SDL game / app
	bool Init();

	// main loop三部曲
	void HandleEvents();
	void Update();
	void Render();

	// Free up resources
	void CleanUp();

	//=========================================================
	//Get方法声明
	//=========================================================
	static int GetWindowWidth() { return m_screenWidth; }
	static int GetWindowHeight() { return m_screenHeight; }
	static float GetScaleX() { return SCALE_X; }
	static float GetScaleY() { return SCALE_Y; }

	SDL_Renderer* GetRenderer() { return Renderer; }
	GameStateMachine* getStateMachine() { return m_pGameStateMachine; }
	
	// 进入main loop的条件：The game is still running
	bool running() { return m_bRunning; }

	void SetGameOverFlag(bool gameover) { m_bGameOver = gameover; }
	bool GetGameOverFlag() { return m_bGameOver; }
	void SetScore(int score) { m_score = score; }
	int GetScore() { return m_score; }

	void quit() { m_bRunning = false; }


private:
	Game() {}

	static Game* pInstance;

	GameStateMachine* m_pGameStateMachine;

	bool m_bRunning = true;
	bool m_bGameOver = false;

	SDL_Window* Window = NULL;
	SDL_Renderer* Renderer = NULL;

	//Screen dimension constants
	static const int m_screenWidth =  512 /*768*/  /*1024*/ /*1536*/;
	static const int m_screenHeight =  384  /*576*/  /*768*/;
	static const float SCALE_X;
	static const float SCALE_Y;
	//clw note：test.xml以768*576为准

	int m_score = 0;
}; 
//clw note 20180819：上面这个分号注意别丢了还不知道。。。

// create the typedef
typedef Game TheGame;

 

#endif