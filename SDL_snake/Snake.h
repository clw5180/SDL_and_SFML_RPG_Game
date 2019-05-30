#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "SDLGameObject.h"
#include "inputhandler.h"
#include "gameobjectfactory.h"
#include "Log.h"

enum MoveDirection 
{ 
	MOVEUP = 1, 
	MOVEDOWN, 
	MOVELEFT, 
	MOVERIGHT 
};

class Snake : public SDLGameObject
{
public:
	Snake();
	virtual ~Snake();

	virtual void load(std::unique_ptr<LoaderParams> const &pParams);

	virtual void update();
	virtual void draw();
	virtual void clean();

	void handleInput();

	virtual std::string type() { return "Player"; }

	//int GetDiameter() { return m_diameter; }

private:
	//=========================================================
	//子程式声明
	//=========================================================
	void RandomGenerateFood(); //随机生成食物的位置
	void moveSnakeByDirection();  //update子函数

	// 小蛇移动方向，上下左右分别用1，2，3，4表示
	int m_moveDirection;

	//velocity = 1相当于1s走m_diameter * 1的距离。因为在Game::Update函数有判断m_count > 60 / m_velocity，由于且Fps是60，即1s会执行60次Update函数；
	int m_velocity = 4;
	static const int m_velocityMax = 32;

	//Setup the clips for our image
	//clw note 20180819：这里是一个结构体数组，每一个clip都含有x,y,w,h四个参数，恰好框选sprite list的一个sprite
	SDL_Rect clips[2];
	//int useClip = 0;
	int m_iW = 0;
	int m_iH = 0;

	int m_DstX = 0;
	int m_DstY = 0;

	//clw note 20180820：这里必须要写成static const的形式
	//                   如果只写static，提示"带有类内初始值设定项的成员必须为常量"；
	//                   如果只写const，那么在类内不能用常量表达式来定义数组（类外可以）；

	static const int m_diameter = 32;
	int m_lineNum;     //窗口内的行数=窗口高度/蛇每一节的尺寸这里是diameter
	int m_columnNum;   //窗口内的列数=窗口宽度/蛇每一节的尺寸这里是diameter

    // 二维数组存储游戏画布中对应的元素
	// 0为空格，-1为边框#，1为蛇头@，大于1的正数为蛇身*
	int **m_pCanvas;
	

	int m_foodY;
	int m_foodX;

	//以此修改进入Update的间隔
	int m_count = 0;
};

class SnakeCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new Snake();
	}
};

#endif