#pragma once

#include <SFML/Graphics.hpp>

class Game : private sf::NonCopyable //TODO
{
public:
	Game();
	void run();

private:
	void processEvents();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void update(sf::Time deltaTime);
	void render();
	
	static const float playerSpeed;
	static const sf::Time timePerFrame;

	sf::RenderWindow mWindow;
	//sf::CircleShape mPlayer;
	sf::Texture mTexture;
	sf::Sprite mPlayer;

	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingLeft;
	bool mIsMovingRight;
};

