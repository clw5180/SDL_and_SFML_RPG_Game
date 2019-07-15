#include <SFML/Graphics.hpp>
// 如果我们用Window类，我们必须使用头文件#include <SFML/Window.hpp >

void initShape(sf::RectangleShape &shape, sf::Vector2f const &pos, sf::Color const &color)
{
	shape.setFillColor(color);
	shape.setPosition(pos);
	shape.setOrigin(shape.getSize() * 0.5f); // The center of the rectangle
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(482, 180), "Bad Squares!");  //创建窗口代码
   // sf::sleep(sf::seconds(3)); //这样我们就可以创建窗口之后，运行代码可以看到窗口，否则不可以

	window.setFramerateLimit(60); //设置每秒目标帧数

	sf::Vector2f startPos = sf::Vector2f(50, 50);
	sf::RectangleShape playerRect(sf::Vector2f(50, 50));
	initShape(playerRect, startPos, sf::Color::Green);

	sf::RectangleShape targetRect(sf::Vector2f(50, 50));
	initShape(targetRect, sf::Vector2f(400, 50), sf::Color::Blue);

	sf::RectangleShape badRect(sf::Vector2f(50, 100));
	initShape(badRect, sf::Vector2f(250, 50), sf::Color::Red);

	while (window.isOpen())
	{
		//Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		//Update frame
		playerRect.move(1, 0); //总是向右移动
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			playerRect.move(0, 1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{
			playerRect.move(0, -1);
		}
		//达到了目标。 你赢了。 退出游戏
		if (playerRect.getGlobalBounds().intersects(targetRect.getGlobalBounds()))
		{
			window.close();
		}
		// 你碰到了敌人，输了，重新开始!
		if (playerRect.getGlobalBounds().intersects(badRect.getGlobalBounds()))
		{
			playerRect.setPosition(startPos);
		}
		//Render frame
		window.clear();
		window.draw(playerRect);
		window.draw(targetRect);
		window.draw(badRect);
		window.display();
	}
	system("pause");
	return 0;
}