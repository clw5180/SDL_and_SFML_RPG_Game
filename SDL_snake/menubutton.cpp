#include "menubutton.h"
#include "inputhandler.h"
#include "SoundManager.h"
#include <string>
#include "Game.h"

//MenuButton::MenuButton(const LoaderParams* pParams, void(*callback)()) : SDLGameObject(pParams), m_callback(callback)
MenuButton::MenuButton() : SDLGameObject(), m_callback(0), m_bReleased(true)
{
	
}

void MenuButton::load(std::unique_ptr<LoaderParams> const &pParams)
{
	SDLGameObject::load(std::move(pParams));
	m_callbackID = pParams->getCallbackID();
	m_currentFrame = MOUSE_OUT;  // start at frame 0
}


void MenuButton::draw()
{
	SDLGameObject::draw(); // use the base class drawing
}

void MenuButton::update()
{
	Vector2D* pMousePos = TheInputHandler::Instance()->getMousePosition();

	//如果鼠标移动到了按钮的范围内
	//if (pMousePos->getX() < (m_position.getX() + m_width) && pMousePos->getX() > m_position.getX()
	//	&& pMousePos->getY() < (m_position.getY() + m_height) && pMousePos->getY() > m_position.getY())                       
	if (pMousePos->getX() < (m_position.getX() + m_width) * Game::GetScaleX()  && pMousePos->getX() > m_position.getX() * Game::GetScaleX()
		&& pMousePos->getY() < (m_position.getY() + m_height) * Game::GetScaleY() && pMousePos->getY() > m_position.getY() * Game::GetScaleY())                       
	{
		//Play the sound effects
		//clw modify 20180918 增加点击playbutton按钮才有音效，其他没有
		if (!m_bMouseMoveToButton)
		{
			SoundManager::Instance()->playSound("MouseMoveToButton", 0);
			m_bMouseMoveToButton = true;
		}

		if (!TheInputHandler::Instance()->getMouseButtonState(LEFT) )  //第1种情况：如果左键是松开的状态
		{
			if (m_bReleased)                   //（1）如果前一时刻左键也是松开的状态
			{
				m_currentFrame = MOUSE_OVER; //说明并没有点击按钮，因此只需换一帧button的图片，表明此button已处于选中状态(尚未点击)
			}
			else                            //（2）如果前一时刻是按下的状态
			{
				//Play the sound effects
				//clw modify 20180918 增加点击playbutton按钮才有音效，其他没有
				if (this->GetTextureID() == string("playbutton"))
					SoundManager::Instance()->playSound("MouseClickButton", 0);

				m_currentFrame = CLICKED; //说明是在按钮上点了左键然后现在松开了，下面就需要准备从mainmenustate向playstate切换

				if (m_callback != 0)    //这里很重要！
				{
					m_callback();     //如果没有上面的判断，且恰好函数指针m_callback为空，就会宕掉！
				}

				m_bReleased = true;
			}
			
		}
		else if (TheInputHandler::Instance()->getMouseButtonState(LEFT))  //第2种情况：如果左键是按下的状态
		{
			m_bReleased = false;   //便于之后在button范围内松开左键时，能够进入执行m_callback()的if-else分支
		}
	}
	else  //鼠标位置在按钮的范围内之外
	{
		m_bReleased = true;
		m_currentFrame = MOUSE_OUT;
		m_bMouseMoveToButton = false;
	}
}

void MenuButton::clean()
{
	SDLGameObject::clean();
}

