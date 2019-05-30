#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <iostream>

class CCamera
{
public:
	static CCamera* Instance()
	{
		if (s_pInstance == NULL)
			s_pInstance = new(CCamera);
		return s_pInstance;
	}

	void Update();

	void SetX(int x) { m_x = x; }
	void SetY(int y) { m_y = y; }
	int GetX() { return m_x; }
	int GetY() { return m_y; }

private:
	CCamera();
	~CCamera();
	
	static CCamera* s_pInstance;
	int m_x = 0;
	int m_y = 0;
};

#endif  /* defined(__CAMERA_H__)*/