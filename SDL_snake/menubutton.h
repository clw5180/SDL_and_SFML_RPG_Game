#ifndef __MENUBUTTON__
#define __MENUBUTTON__

#include "SDLGameObject.h"
#include "gameobjectfactory.h"

//============================================================
//类名称：    MenuButton
//说明：      菜单按钮类
//日期：      2018-09
//修改记录：  修改了update内的按钮点击处理逻辑，保证在按钮范围内
//            点击后松开，才触发mainmenustate-playstate状态切换
//============================================================
class MenuButton : public SDLGameObject
{
public:
	MenuButton();

	virtual void load(std::unique_ptr<LoaderParams> const &pParams);  //clw note：std::unique_ptr<LoaderParams>？？

	virtual void draw();
	virtual void update();
	virtual void clean();

	void setCallback(void(*callback)()) { m_callback = callback; }
	int getCallbackID() { return m_callbackID; }

	

private:
	enum button_state
	{
		MOUSE_OUT = 0,
		MOUSE_OVER = 1,
		CLICKED = 2
	};

	bool m_bReleased;

	int m_callbackID;

	void (*m_callback)();

	//add by clw 20180918
	bool m_bMouseMoveToButton = false;
};

class MenuButtonCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new MenuButton();
	}
};

#endif
