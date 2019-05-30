#ifndef __INPUTHANDLER__
#define __INPUTHANDLER__

#include <vector>

#include "SDL.h"
#include "Vector2D.h"

enum mouse_buttons
{
	LEFT = 0,
	MIDDLE = 1,
	RIGHT = 2
};

class InputHandler
{
public:
	static InputHandler* Instance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new InputHandler();
		}

		return s_pInstance;
	}

	// init joysticks
	void initialiseJoysticks();
	bool joysticksInitialised() const { 
		return m_bJoysticksInitialised; 
	}

	void reset();

	// update and clean the input handler
	void update(); //poll for events and update  InputHandler accordingly
	void clean();  //clear any devices we have initialized

	// keyboard events
	bool isKeyDown(SDL_Scancode key) const;
	bool isKeyRepeat() { return m_bKeyRepeat; }

	// joystick events
	float getAxisX(int joy, int stick) const;
	float getAxisY(int joy, int stick) const;
	//bool getButtonState(int joy, int buttonNumber) const;

	// mouse events
	bool getMouseButtonState(int buttonNumber) const;
	Vector2D* getMousePosition() const;

private:
	InputHandler();
	~InputHandler();

	// handle keyboard events
	void onKeyDown();
	void onKeyUp();

	// handle mouse events
	void onMouseMove(SDL_Event& event);
	void onMouseButtonDown(SDL_Event& event);
	void onMouseButtonUp(SDL_Event& event);

	// handle joysticks events
	void onJoystickAxisMove(SDL_Event& event);
	void onJoystickButtonDown(SDL_Event& event);
	void onJoystickButtonUp(SDL_Event& event);

	// member variables

	// keyboard specific
	const Uint8* m_keystates;  
	// add by clw 20180916，如果上下左右同时有多于一个键按下了，那么即使对某个键来说event.key.repeat == 1，但是对于下面这个变量而言，已经不算重复按键了，因为改变了方向；
	bool m_bKeyRepeat = false;

	// joystick specific
	std::vector<std::pair<Vector2D*, Vector2D*>> m_joystickValues;
	std::vector<SDL_Joystick*> m_joysticks;
	std::vector<std::vector<bool>> m_buttonStates;
	bool m_bJoysticksInitialised;
	static const int m_joystickDeadZone = 10000;

	// mouse specific
	std::vector<bool> m_mouseButtonStates;
	Vector2D* m_mousePosition;

	// singleton
	static InputHandler* s_pInstance;
	
};
typedef InputHandler TheInputHandler;


#endif

