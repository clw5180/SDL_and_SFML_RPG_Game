#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>

class CGame
{
public:
	static CGame* Instance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CGame();
		}
		return s_pInstance;
	}

	bool Init(); //初始化
	void HandleEvents(); //输入事件处理，包括鼠标、键盘、手柄等
	void Update();  //根据不同的事件对游戏状态或内容进行更新
	void Render();  //渲染，完成图像的绘制
	void Close();  //释放内存，结束

	bool Running() { return m_bRunning; }
	void Quit() { m_bRunning = false; }

private:
	static CGame* s_pInstance;

	CGame();
	~CGame();

	bool m_bRunning = true;
};

#endif   /* defined(__GAME_H__)*/