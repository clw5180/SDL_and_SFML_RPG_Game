#include "AssetManager.h"
#include <assert.h>

AssetManager* AssetManager::sInstance = nullptr;

AssetManager::AssetManager()
{
	// Only allow one AssetManager to exist
	// Otherwise throw an exception
	assert(sInstance == nullptr);
	sInstance = this;
}

sf::Texture& AssetManager::GetTexture(std::string const& filename)
{
	auto& texMap = sInstance->m_Textures;

	// See if the texture is already loaded
	auto pairFound = texMap.find(filename);
	// If yes, return the texture
	if (pairFound != texMap.end())
	{
		return pairFound->second;
	}
	else  // Else, load the texture and return it
	{
		// Create an element in the texture map
		auto& texture = texMap[filename];
		texture.loadFromFile(filename);
		return texture;
	}
}
