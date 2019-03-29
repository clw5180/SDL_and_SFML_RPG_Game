#ifndef __MAP_H__
#define __MAP_H__

#include <iostream>

class CMap
{
public:
	static CMap* Instance()
	{
		if (s_pInstance == NULL)
			s_pInstance = new CMap();
		return s_pInstance;
	}

	int GetX() { return m_x; }
	int GetY() { return m_y; }
	void Update();
	void Render();

private:
	CMap();
	~CMap();
	
	static CMap* s_pInstance;

	int m_x = -1146;
	int m_y = -412; //clw note：for debug，也可以是其它位置，比如（0,0）
};

#endif  /* defined(__MAP_H__)*/