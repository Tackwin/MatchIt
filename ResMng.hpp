#pragma once
#include <map>

#define BACKGORUNDHALF "Res/BackgroundHalf.png"
#define HOVERBARLIFE "Res/hoverBarLife.png"
#define FULLBARLIFE "Res/fullBarLife.png"
#define HOVERBARCOUNTDOWN "Res/hoverBarCountdown.png"
#define FULLBARCOUNTDOWN "Res/fullBarCountdown.png"
#define GAMEOVER "Res/gameOver.png"
#define MUSIC "Res/music.ogg"
#define WAITING "Res/waitingScreen.png"
#define PREVIEW "Res/preview.png"
#define MENU "Res/menu.png"
#define ITEM "Res/item.png"
#define FAIL "Res/fail.png"
#define FAILSONG "Res/failSound.ogg"
#define IDLE "Res/iddle.ogg"
#define FONT "Res/f.otf"
#define ACTION "Res/action.png"

namespace sf {
	class Texture;
	class String;
	//class Shader;
	class Image;
	class Font;
	class SoundBuffer;
	class Music;
};

class ResMng {
private:
	ResMng();
	~ResMng();

	static std::map<sf::String, sf::SoundBuffer> sounds;
	static std::map<sf::String, sf::Texture> textures;
	//static std::map<sf::String, sf::Shader> shaders;
	static std::map<sf::String, sf::Music> musics;
	static std::map<sf::String, sf::Image> images;
	static std::map<sf::String, sf::Font> fonts;
public:
	static sf::SoundBuffer& getSound(const sf::String &path);
	static sf::Texture& getTexture(const sf::String &path);
	//static sf::Shader& getShader(const sf::String &path);
	static sf::Music& getMusic(const sf::String &path);
	static sf::Image& getImage(const sf::String &path);
	static sf::Font& getFont(const sf::String &path);
	static void loadAll();
};

