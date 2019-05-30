#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "LoaderParams.h"
#include "Vector2D.h"
#include <string>
#include <memory>

using namespace std;

class GameObject
{
public:

	// base class needs virtual destructor
	virtual ~GameObject() {}

	// load from file - int x, int y, int width, int height, std::string textureID, int numFrames, int callbackID = 0, int animSpeed = 0
	virtual void load(std::unique_ptr<LoaderParams> const &pParams) = 0;

	// do update stuff
	virtual void update() = 0;

	// draw the object
	virtual void draw() = 0;

	// remove anything that needs to be deleted
	virtual void clean() = 0;

	// get the type of the object
	virtual std::string type() = 0;

	// getters for common variables
	Vector2D& getPosition() { return m_position; }
	//Vector2D& getVelocity() { return m_velocity; }  //clw note：暂时弃用速度，以后再修改

	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	string& GetTextureID() { return m_textureID; }  //add by clw 20180918

	// is the object currently being updated?
	bool updating() { return m_bUpdating; }

	// set whether to update the object or not
	void setUpdating(bool updating) { m_bUpdating = updating; }

protected:

	// constructor with default initialisation list
	GameObject() :  //m_position(0, 0),
					//m_velocity(0, 0),
					//m_acceleration(0, 0),
					//m_width(0),
					//m_height(0),
					m_currentRow(1),
				    m_currentFrame(0)
	{
	}

	// movement variables
	Vector2D m_position;
	//Vector2D m_velocity;

	// size variables
	int m_width;
	int m_height;

	// animation variables
	int m_currentRow;
	int m_currentFrame;
	int m_numFrames;
	string m_textureID;

	// common boolean variables
	bool m_bUpdating;
};

#endif
