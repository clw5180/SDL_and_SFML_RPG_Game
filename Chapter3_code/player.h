#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>

class CPlayer
{
public:
	static CPlayer* Instance()
	{
		if (s_pInstance == NULL)
			s_pInstance = new CPlayer();
		return s_pInstance;
	}

	int GetX() { return m_x; }
	int GetY() { return m_y; }
	int GetWidth() { return m_w; }
	int GetHeight() { return m_h; }

	void Render();
	void Update();

private:
	CPlayer();
	~CPlayer();

	static CPlayer* s_pInstance;

	enum PlayerDirection
	{
		PLAYERLEFT,
		PLAYERRIGHT,
		PLAYERUP,
		PLAYERDOWN,
		PLAYERLEFTDOWN,
		PLAYERRIGHTUP,
		PLAYERLEFTUP,
		PLAYERRIGHTDOWN,
	};


	// 和动作相关的变量
	int m_currentRow = 0;  //当前帧处于精灵表（Sprite Sheet）的第多少行
	int m_currentFrame = 0; //当前帧处于精灵表（Sprite Sheet）的第多少列
	int m_numFrames = 8;  
	int m_numRows = 9;  //准备载入8列，9行的角色精灵表     TODO：后面考虑修改这种写法?
	int m_movedirection = PLAYERDOWN;
	int m_bWalkState = true;

	// 窗口中角色的宽高和角色的位置
	int m_x = 0;
	int m_y = 0;
	int m_w = 448 / m_numFrames; //TODO，这里暂时hard code图片的宽高尺寸，后面加入纹理管理器类，
						   //每次Load图片时就会管理好每一张图片的尺寸等信息
	int m_h = 819 / m_numRows;  //TODO

	
};

#endif  /* defined(__PLAYER_H__)*/