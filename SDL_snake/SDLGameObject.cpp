#include "SDLGameObject.h"
#include "TextureManager.h"
#include "Game.h"  //如果放在SDLGameObject.h将会形成闭合环状！！

//SDLGameObject::SDLGameObject(const LoaderParams* pParams) : GameObject(pParams), m_position(pParams->getX(), pParams->getY()), m_velocity(0,0)
//{
//	m_width = pParams->getWidth();
//	m_height = pParams->getHeight();
//
//	m_textureID = pParams->getTextureID();
//
//	m_currentRow = 1;
//	m_currentFrame = 1;
//}

SDLGameObject::SDLGameObject() : GameObject()
//,m_bulletFiringSpeed(0),
//m_bulletCounter(0),
//m_moveSpeed(0),
//m_dyingTime(0),
//m_dyingCounter(0),
//m_bPlayedDeathSound(false)
{

}

//void SDLGameObject::load(const LoaderParams *pParams)
void SDLGameObject::load(std::unique_ptr<LoaderParams> const &pParams)
{
	m_position = Vector2D((float)pParams->getX(), (float)pParams->getY());

	//m_velocity = Vector2D(0, 0);   
	//clw note
    //m_acceleration = Vector2D(0, 0);

	m_width = pParams->getWidth();
	m_height = pParams->getHeight();

	m_textureID = pParams->getTextureID();
	m_numFrames = pParams->getNumFrames(); //应该有
}

void SDLGameObject::update()
{
	//m_position += m_velocity;
}

void SDLGameObject::draw()
{
//  clw modify 20190201：
//  考虑增加根据不同大小窗口进行缩放的功能，
//  test.xml内的值是以768*576窗口大小为基准的。
//  实际窗口大小如果不同，需要按比例对object进行缩放

	//TextureManager::Instance()->drawFrame(m_textureID, (int)m_position.getX(), (int)m_position.getY(), m_width, m_height, m_currentRow, m_currentFrame, TheGame::Instance()->GetRenderer());
	TextureManager::Instance()->drawFrameEx(m_textureID, m_width, m_height, 
		(int)(m_position.getX() * Game::GetScaleX()), 
		(int)(m_position.getY() * Game::GetScaleY()), 
		m_width * Game::GetScaleX(), m_height * Game::GetScaleY(), 
		m_currentRow, m_currentFrame, TheGame::Instance()->GetRenderer());
}

void SDLGameObject::clean()
{

}

