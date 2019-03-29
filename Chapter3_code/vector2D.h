#ifndef __Vector2D_H__
#define __Vector2D_H__

#include <iostream>
#include <math.h>

class Vector2D
{
public:
    Vector2D()
    {
        m_x = 0;
        m_y = 0;
    }
    
    Vector2D(int x, int y): m_x(x), m_y(y) {}
    
    const int GetX() { return m_x; }
    const int GetY() { return m_y; }
    
    void SetX(int x) { m_x = x; }
    void SetY(int y) { m_y = y; }
    
    int Length() 
	{ 
		return (int)sqrt(m_x * m_x + m_y * m_y); 
	}
    
    Vector2D operator+(const Vector2D& v2) const 
	{ 
		return Vector2D(m_x + v2.m_x, m_y + v2.m_y); 
	}

    friend Vector2D& operator+=(Vector2D& v1, const Vector2D& v2)
    {
        v1.m_x += v2.m_x;
        v1.m_y += v2.m_y;
        
        return v1;
    }
    
    Vector2D operator-(const Vector2D& v2) const 
	{ 
		return Vector2D(m_x - v2.m_x, m_y - v2.m_y); 
	}

    friend Vector2D& operator-=(Vector2D& v1, const Vector2D& v2)
    {
        v1.m_x -= v2.m_x;
        v1.m_y -= v2.m_y;
        
        return v1;
    }

    Vector2D operator*(int scalar)
    {
        return Vector2D(m_x * scalar, m_y * scalar);
    }
    
    Vector2D& operator*=(int scalar)
    {
        m_x *= scalar;
        m_y *= scalar;
        
        return *this;
    }
    
    Vector2D operator/(int scalar)
    {
        return Vector2D(m_x / scalar, m_y / scalar);
    }
    
    Vector2D& operator/=(int scalar)
    {
        m_x /= scalar;
        m_y /= scalar;
        
        return *this;
    }

    
    void Normalize()
    {
        int l = Length();
        if ( l > 0)
        {
            (*this) *= 1 / l;
        }
    }
    
private:
	int m_x;
	int m_y;
};

#endif /* defined(__Vector2D_H__) */
