#include <SFML/Graphics.hpp>
#include "AssetManager.h"

int main() 
{
	sf::RenderWindow window(sf::VideoMode(640, 480), "AssetManager");
	AssetManager manager;
	
	// Create sprites
	sf::Sprite sprite1 = sf::Sprite(AssetManager::GetTexture("myTexture1.png"));
	sf::Sprite sprite2 = sf::Sprite(AssetManager::GetTexture("myTexture2.png"));
	sf::Sprite sprite3 = sf::Sprite(AssetManager::GetTexture("myTexture3.png"));
	
	while (window.isOpen()) 
	{
	// Game loop
	}
	
	// After main() returns, the manager is destroyed
	return 0;
}
