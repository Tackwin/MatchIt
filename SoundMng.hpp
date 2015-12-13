#pragma once
#include <SFML\Audio.hpp>
#include <queue>
class SoundMng {
private:
	SoundMng();
	~SoundMng();

	static std::queue<sf::Sound> sounds;
public:
	static int addSound(sf::SoundBuffer &buffer, float volume = 100.f);

	static void update();
};

