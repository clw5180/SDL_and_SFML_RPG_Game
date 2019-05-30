#include "Snake.h"
#include "Game.h"
#include "TextureManager.h"


Snake::Snake() : SDLGameObject()
{
	m_lineNum = TheGame::Instance()->GetWindowHeight() / m_diameter;
	m_columnNum = TheGame::Instance()->GetWindowWidth() / m_diameter;
	//根据屏幕宽度和snake的尺寸（每一节的直径）来定义二维数组，用于存储snake的位置等信息
	m_pCanvas = new int *[m_lineNum];
	for (int i = 0; i < m_lineNum; ++i)
		m_pCanvas[i] = new int[m_columnNum]();
}

Snake::~Snake()
{
	if (m_pCanvas != NULL)
	{
		delete m_pCanvas;
		m_pCanvas = NULL;
	}
}

//=========================================================
//函数名称：load
//说明：
//修改记录：
//=========================================================
void Snake::load(std::unique_ptr<LoaderParams> const &pParams)
{
	//SDLGameObject::load(std::move(pParams));

	m_iW = 100, m_iH = 100;
	for (int i = 0; i < 2; ++i)
	{
		//选取./res/image.png的第一列的两个sprite，即蓝色和黄色的圆
		clips[i].x = m_iW;
		clips[i].y = i % 2 * m_iH;
		clips[i].w = m_iW;
		clips[i].h = m_iH;
	}

	/*
	clw note 20180820：
	设计如下：窗口1024*768，圆外切正方形大小为32*32，一行可以放32个圆，一列可以放24个圆
	初始化边框
	这里注意一个很重要的问题：把数组第一维i当做y轴方向。
	—— —— —— —— —— —— → x轴
	|
	|
	|
	↓
	y轴
	*/
	for (int i = 0; i < m_lineNum; i++)   //注：这里纵向初始化
	{
		m_pCanvas[i][0] = -1;
		m_pCanvas[i][m_columnNum - 1] = -1;
	}

	for (int j = 0; j < m_columnNum; j++)  //注：这里横向初始化
	{
		m_pCanvas[0][j] = -1;
		m_pCanvas[m_lineNum - 1][j] = -1;
	}

	// 初始化蛇头位置
	m_pCanvas[m_lineNum / 2][m_columnNum / 2] = 1;

	// 初始化蛇身，画布中元素值分别为2,3,4,5....
	for (int i = 1; i <= 4; i++)
	{
		m_pCanvas[m_lineNum / 2][m_columnNum / 2 - i] = i + 1;
	}

	m_moveDirection = MOVERIGHT;

	RandomGenerateFood();

	//clw note：注意m_foodX比较小，因此在canvas前面
	m_pCanvas[m_foodY / m_diameter][m_foodX / m_diameter] = -2;
}

//=========================================================
//函数名称：handleInput
//说明：
//修改记录：
//=========================================================
void Snake::handleInput()
{
	// handle keys
	if ( !(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) ||
		   TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) ||
		   TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) ||
		   TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT)) 
	     || !TheInputHandler::Instance()->isKeyRepeat() )
	{
		m_velocity = 4;
	}

	if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) 
		/*&& m_position.getY() > 0*/)  //按了UP键
	{
		//m_velocity.setY(-m_moveSpeed);

		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVEDOWN)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		/*clw note 20180914 逻辑：如果此时是朝某一方向走，则应该保证按相反的方向键无效，因为撞到自己的行为不能Update，也不能算失败。。
		这里用到下面的if判断，但如果只在这里判断还会有一个问题：
		1、比如snake正在向下运动，虽然此时按了上，由于m_moveDirection为MOVEDOWN，因此由于下面if的存在不会把m_moveDirection写成MOVEUP，也就不会在之后撞到自己，这是合理的；
		2、但是，如果此时按了右，再迅速按上，由于snake还没有来得及向右运动，但m_moveDirection已经变成MOVERIGHT了而不再是MOVEDOWN，因此m_moveDirection会写成MOVEUP，从而导致之后Update时，snake反向运动撞到自己，导致游戏失败，这是不合理的。因此，还需要在Update里面再进行自身碰撞检测, 即如果自己和自己撞直接return什么都不做即可；（如果用多线程能否解决这一问题？）

		另外注意，下面的if判断也是不能删的，因为Update由于if (m_count > 60 / m_velocity)的存在，相当于每隔1/m_velocity秒才会进入一次，
		所以如果删了if判断，那么在没进入Update而进入几次HandleEvents的时候，就会出现按反向键也会使snake加速的情况。
		*/
		if (m_moveDirection != MOVEDOWN)
			m_moveDirection = MOVEUP;
		return;  //比如同时按了上和右两个键位，键值表m_keystates里存了上和右，因为if判断的顺序是上下左右的顺序，因此会优先处理上这个事件然后return。
	}

	else if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN))  
	{
		
		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVEUP)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		if (m_moveDirection != MOVEUP)
			m_moveDirection = MOVEDOWN;
		return;

	}
		
	else if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT))
	{
		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVERIGHT)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		if (m_moveDirection != MOVERIGHT)
			m_moveDirection = MOVELEFT;
		return;
	}

	else if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT))
	{
		if (TheInputHandler::Instance()->isKeyRepeat() && m_moveDirection != MOVELEFT)
		{
			m_velocity *= 4;
			if (m_velocity >= m_velocityMax)
				m_velocity = m_velocityMax;
		}
		if (m_moveDirection != MOVELEFT)
			m_moveDirection = MOVERIGHT;
		return;
	}
}

//=========================================================
//函数名称：update
//说明：
//修改记录：
//=========================================================
void Snake::update()
{
	handleInput();

	//SDLGameObject::update();  //clw note：这是一个通用的update方法

	if (m_count > 60 / m_velocity)
	{
		moveSnakeByDirection();
	}
	else
	{
		m_count++;
	}

}

//=========================================================
//函数名称：draw
//说明：
//修改记录：
//=========================================================
void Snake::draw()
{
	
	//clw note 20180820：SDL_RenderClear函数会采用SDL_SetRenderDrawColor函数设置的颜色填充renderer；
	//		   注意，RGB值若R=0,G=0,B=0，相当于没有光，屏幕不亮，自然是黑色；
	//		   而太阳光约等于白色，可以从太阳光折射出七彩，因此R=255,G=255,B=255对应为白色。

	//draw snake and food
	for (int i = 0; i < m_lineNum; i++)
	{
		for (int j = 0; j < m_columnNum; j++)
		{
			//clw note：在res里面有个命名为snake的图，里面有四个圆
			//          用黄色的圆表示food，用蓝色的圆表示snake
			if (m_pCanvas[i][j] == -2)
			{
				//render food
				//TextureManager::Instance()->GetTextureFromList("snake")->RenderTexture(TextureManager::Instance()->GetTextureFromList("snake")->GetSDLTexture(), Renderer, j * m_diameter, i * m_diameter, &clips[1]);
				TextureManager::Instance()->drawClip("snake", j*m_diameter, i*m_diameter, m_diameter, TheGame::Instance()->GetRenderer(), &clips[1]);
			}

			else if (m_pCanvas[i][j] != 0 && m_pCanvas[i][j] != -1 && m_pCanvas[i][j] != -2)
			{
				//render snake
				//TextureManager::Instance()->GetTextureFromList("snake")->RenderTexture(TextureManager::Instance()->GetTextureFromList("snake")->GetSDLTexture(), Renderer, j * m_diameter, i * m_diameter, &clips[0]);
				TextureManager::Instance()->drawClip("snake", j*m_diameter, i*m_diameter, m_diameter, TheGame::Instance()->GetRenderer(), &clips[0]);
			}
		}
	}

	//render score
	//clw note 20180824：分数Score的位置，这里要保证左对齐，所以最好不要做减法？
	SDL_Texture* tempTex = TextureManager::Instance()->GetTextureFromList("score");
	SDL_Rect dst;
	//Get dimensions of text Texture
	SDL_QueryTexture(tempTex, NULL, NULL, &dst.w, &dst.h);
	//TextureManager::Instance()->RenderTexture(tempTex, TheGame::Instance()->GetRenderer(), int(TheGame::Instance()->GetWindowWidth() * 0.7), dst.h * 2, m_diameter, NULL);
	TextureManager::Instance()->RenderTexture(tempTex, TheGame::Instance()->GetRenderer(), int(TheGame::Instance()->GetWindowWidth() - 200), dst.h * 2, m_diameter, NULL);
}

//=========================================================
//函数名称：clean
//说明：
//修改记录：
//=========================================================
void Snake::clean()
{
	SDLGameObject::clean();
}

//==============以下为子函数==============//
void Snake::RandomGenerateFood()
{
	int tmpXMax = (TheGame::Instance()->GetWindowWidth() - 2 * m_diameter) / m_diameter;
	int tmpYMax = (TheGame::Instance()->GetWindowHeight() - 2 * m_diameter) / m_diameter;
	m_foodX = (rand() % tmpXMax + 1) * m_diameter;
	m_foodY = (rand() % tmpYMax + 1) * m_diameter;
}

void Snake::moveSnakeByDirection()
{
	int i, j;

	int oldTail_i, oldTail_j, oldHead_i, oldHead_j;
	int max = 0;

	for (i = 1; i < m_lineNum - 1; i++)
	{
		for (j = 1; j < m_columnNum - 1; j++)
		{
			if (m_pCanvas[i][j] > 0)
			{
				//找到移动之前的尾位置，比如之前是5 4 3 2 1
				//就是5的位置
				if (max < m_pCanvas[i][j])
				{
					max = m_pCanvas[i][j];
					oldTail_i = i;
					oldTail_j = j;
				}
				if (m_pCanvas[i][j] == 1)
				{
					//找到移动前的头位置，比如之前是5 4 3 2 1
					//就是1的位置
					oldHead_i = i;
					oldHead_j = j;
				}
			}
		}
	}

	//canvas[oldTail_i][oldTail_j] = 0;  //没加食物之前用
	int newHead_i, newHead_j;

	// clw note 20180824：可以做一个函数moveSnakeByDirection()
	switch (m_moveDirection)
	{
	case MOVEUP:  // 向上移动
				  //clw modify 20180823
				  //如果是snake往相反方向运动，比如向上移动导致和自己撞上了，不能算和自身撞，不是游戏失败
				  //直接把方向置为相反方向，即向下，然后return
		if (m_pCanvas[oldHead_i - 1][oldHead_j] == 2)
		{
			m_moveDirection = MOVEDOWN;
			return;
		}

		//canvas[oldHead_i - 1][oldHead_j] = 1;  //没加食物之前用
		newHead_i = oldHead_i - 1;
		newHead_j = oldHead_j;
		break;
	case MOVEDOWN:
		if (m_pCanvas[oldHead_i + 1][oldHead_j] == 2)
		{
			m_moveDirection = MOVEUP;
			return;
		}

		//canvas[oldHead_i + 1][oldHead_j] = 1;  //没加食物之前用
		newHead_i = oldHead_i + 1;
		newHead_j = oldHead_j;
		break;
	case MOVELEFT:
		if (m_pCanvas[oldHead_i][oldHead_j - 1] == 2)
		{
			m_moveDirection = MOVERIGHT;
			return;
		}

		//canvas[oldHead_i][oldHead_j - 1] = 1;  //没加食物之前用
		newHead_i = oldHead_i;
		newHead_j = oldHead_j - 1;
		break;
	case MOVERIGHT:
		if (m_pCanvas[oldHead_i][oldHead_j + 1] == 2)
		{
			m_moveDirection = MOVELEFT;
			return;
		}

		//canvas[oldHead_i][oldHead_j + 1] = 1;  //没加食物之前用
		newHead_i = oldHead_i;
		newHead_j = oldHead_j + 1;
		break;
	}

	for (i = 1; i < m_lineNum - 1; i++)
	{
		for (j = 1; j < m_columnNum - 1; j++)
		{
			if (m_pCanvas[i][j] > 0)
				m_pCanvas[i][j]++;
			//无论向哪里移动，首先把snake的所有部位+1，表示移动
			//再根据方向确定snake head的位置
			//例如，开始的时候是5 4 3 2 1 （边界都是-1，其他地方是0）
			//现在变成了6 5 4 3 2。
		}
	}

	// 新蛇头如果吃到食物, 原来的旧蛇尾留着，长度自动+1
	if (m_pCanvas[newHead_i][newHead_j] == -2)
	{
		m_pCanvas[m_foodY / m_diameter][m_foodX / m_diameter] = 0;
		// 产生一个新的食物
		//m_foodY = rand() * m_diameter % (m_screenWidth - 5 * m_diameter) + 2 * m_diameter;
		//m_foodX = rand() * m_diameter % (m_screenHeight - 5 * m_diameter) + 2 * m_diameter;

		// clw modify 20180918：生成m_diameter ~ WindowWidth - m_diameter的随机数m_foodX，
		//                        和m_diameter ~ WindowHeight- m_diameter的随机数m_foodY，防止在边界或边界以外的地方生成食物 
		RandomGenerateFood();


		m_pCanvas[m_foodY / m_diameter][m_foodX / m_diameter] = -2;

		//clw modify 20180824 如果吃到食物，分数+10
		//Render text
		int tmpScore = TheGame::Instance()->GetScore();
		tmpScore += 10;
		TheGame::Instance()->SetScore(tmpScore);
		string strScore = "score : " + to_string(tmpScore);
		SDL_Color textColor = { 255, 255, 255 };
		if (TextureManager::Instance()->LoadFromRenderedText(TheGame::Instance()->GetRenderer(), "score", strScore, textColor) == false)
		{
			Log("Unable to LoadFromRenderedText Texture: %s", "score");
		}
	}
	else // 否则，原来的旧蛇尾减掉，保持长度不变
		m_pCanvas[oldTail_i][oldTail_j] = 0;

	// 是否小蛇和自身撞，或者和边框撞，游戏失败
	if (m_pCanvas[newHead_i][newHead_j] > 0 || m_pCanvas[newHead_i][newHead_j] == -1)
	{
		//clw modify 20180917：使用游戏状态机，将游戏进行状态转换到游戏结束状态
		// Q:是否应该加在PlayState.cpp里面，然后这里只是修改一个变量，如当前生命值为0？？ 
		TheGame::Instance()->SetGameOverFlag(true);

		//system("pause");
		//exit(0);  
	}
	else
		m_pCanvas[newHead_i][newHead_j] = 1;

	m_count = 0;
}