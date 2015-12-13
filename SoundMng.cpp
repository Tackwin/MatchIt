#include "SoundMng.hpp"

std::queue<sf::Sound> SoundMng::sounds;

SoundMng::SoundMng() {
}

SoundMng::~SoundMng() {
}

int SoundMng::addSound(sf::SoundBuffer &buffer, float volume) {
	sounds.push(sf::Sound(buffer));
	sounds.back().setVolume(volume);
	sounds.back().play();
	return sounds.size() - 1;
}

void SoundMng::update() {
	if(sounds.empty()) return;
	if(sounds.front().getStatus() == sf::SoundSource::Stopped) sounds.pop();
}