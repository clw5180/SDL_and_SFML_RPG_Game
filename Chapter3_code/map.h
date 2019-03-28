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

	void Update();
	void Render();

	void SetX(int x) { m_x = x; }
	void SetY(int y) { m_y = y; }
	int GetX() { return m_x; }
	int GetY() { return m_y; }

private:
	CMap();
	~CMap();
	
	static CMap* s_pInstance;
	//int m_x = -1296;  //clw note：for debug，可以是其它位置，比如（0,0）
	int m_x = -50;
	int m_y = -552;
};

#endif  /* defined(__MAP_H__)*/