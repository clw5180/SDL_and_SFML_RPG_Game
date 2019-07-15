#include <SFML/Graphics.hpp>

struct Vertex {
	sf::Vector2f Position;
	sf::Vector2f TexCoord;
};

int main01() {
	sf::RenderWindow window(sf::VideoMode(640, 480), "Textures");
	// Set target Frames per second
	window.setFramerateLimit(60);

	sf::Texture texture;
	texture.loadFromFile("cube.png");
	// Set the texture in repeat mode
	//texture.setRepeated(true);

	sf::RectangleShape rectShape(sf::Vector2f(128*3, 221*3));
	// Bigger texture rectangle than the size of the texture
	rectShape.setTextureRect(sf::IntRect(0, 0, 128*3, 221*3));
	rectShape.setTexture(&texture);

	while (window.isOpen()) {
		// Handle events
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::EventType::Closed:
				window.close();
				break;
			case sf::Event::EventType::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape) window.close();
				break;
			default:
				break;
			}
		}

		window.clear(sf::Color::Black);
		window.draw(rectShape);
		window.display();
	}

	return 0;
}
