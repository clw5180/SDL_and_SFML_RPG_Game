#include "game.h"

const float Game::playerSpeed = 200.f; //相当于每秒钟走200个pixels
const sf::Time Game::timePerFrame = sf::seconds(1.f / 60.f);

Game::Game(): mWindow(sf::VideoMode(/*640*/ 1000, 480), "SFML Application"), mTexture(), mPlayer()
{
	if (!mTexture.loadFromFile("../res/Eagle.png"))
	{
		printf("clw: mTexture.loadFromFile error!");
		//Handle loading error
	}
	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(100.f, 100.f);

	//mPlayer.setRadius(40.f);
	//mPlayer.setPosition(100.f, 100.f);
	//mPlayer.setFillColor(sf::Color::Cyan);
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			// Player pressed key. Handle the input
			handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			// Player released key. Handle the input
			handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			// Window was requested to be closed. Close it.
			mWindow.close();
			break;
		}
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	// Store the keyboard input from the user

	if (key == sf::Keyboard::W)
	{
		mIsMovingUp = isPressed;
	}
	else if (key == sf::Keyboard::S)
	{
		mIsMovingDown = isPressed;
	}
	else if (key == sf::Keyboard::A)
	{
		mIsMovingLeft = isPressed;
	}
	else if (key == sf::Keyboard::D)
	{
		mIsMovingRight = isPressed;
	}
}

void Game::update(sf::Time deltaTime)
{ 
	sf::Vector2f movement(0.f, 0.f);
		if (mIsMovingUp)
			movement.y -= playerSpeed;
		if (mIsMovingDown)
			movement.y += playerSpeed;
		if (mIsMovingLeft)
			movement.x -= playerSpeed;
		if (mIsMovingRight)
			movement.x += playerSpeed;

		mPlayer.move(movement * deltaTime.asSeconds());
}

void Game::render()
{
	mWindow.clear();
	mWindow.draw(mPlayer);
	mWindow.display();
}

void Game::run()
{
	// The method that contains the main game loop.
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		// While the window is open, process any events, update the game, and render the display
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame) //clw note：只有过了超过1帧的时间，才进行更新（如位置信息）
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		render(); 
		//如果渲染速度慢，可能会多次进入上面的while循环，画面看起来就是一顿一顿的；
		//如果渲染速度快，则多次调用render()，中间没有逻辑更新；多次渲染相同的状态不会改变屏幕上的任何内容，
	}
}