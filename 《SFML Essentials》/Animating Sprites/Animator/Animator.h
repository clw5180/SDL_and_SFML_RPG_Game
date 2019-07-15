#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <SFML/Graphics.hpp>
#include <list>
#include <string>

class Animator {
public:
	struct Animation {
		std::string m_Name;
		std::string m_TextureName;
		std::vector<sf::IntRect> m_Frames;
		sf::Time m_Duration;
		bool m_Looping;

		Animation(std::string const& name, std::string const& textureName,
			sf::Time const& duration, bool looping)
			: m_Name(name),
			m_TextureName(textureName),
			m_Duration(duration),
			m_Looping(looping) {}

		// Adds frames horizontally
		void AddFrames(sf::Vector2i const& startFrom, sf::Vector2i const& frameSize,
			unsigned int frames) {
			sf::Vector2i current = startFrom;
			for (unsigned int i = 0; i < frames; i++) {
				// Add current frame from position and frame size
				m_Frames.push_back(sf::IntRect(current.x, current.y, frameSize.x, frameSize.y));
				// Advance current frame horizontally
				current.x += frameSize.x;
			}
		}
	};

	Animator(sf::Sprite& sprite);

	Animator::Animation& CreateAnimation(std::string const& name,
		std::string const& textureName,
		sf::Time const& duration, bool loop = false);

	void Update(sf::Time const& dt);

	// Returns if the switch was successful
	bool SwitchAnimation(std::string const& name);

	std::string GetCurrentAnimationName() const;

private:
	// Returns the animation with the passed name
	// Returns nullptr if no such animation is found
	Animator::Animation* FindAnimation(std::string const& name);

	void SwitchAnimation(Animator::Animation* animation);

	// Reference to the sprite
	sf::Sprite& m_Sprite;
	sf::Time m_CurrentTime;
	std::list<Animator::Animation> m_Animations;
	Animator::Animation* m_CurrentAnimation;
};

#endif