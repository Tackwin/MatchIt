#pragma once
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\System\Clock.hpp>
#include <SFML\Audio.hpp>
#include <entityx\entityx.h>
#include <random>
class Game {
private:
	static sf::Clock deltaClock;
public:
	static sf::Clock elapsedClock;
	static sf::Clock countdownClock;
	static sf::Clock shakeClock;
	static sf::RenderWindow window;
	static entityx::EntityX entx;
	static float DELTA;

	static sf::Color colorRight;
	static sf::Color nextColorRight;
	static sf::Color colorLeft;
	static sf::Color nextColorLeft;

	static sf::View view;

	static std::default_random_engine randGenrator;
	static std::uniform_int_distribution<int> colorRandomizer;

	static float life;
	static int score;
	static int scoreMutliplier;
	static int bestCombo;

	static entityx::Entity cam;

	//static sf::Music music;

	Game();
	~Game();

	static void update();
	static void create();
	static void launchGame();
	static void shakeCam();

	static inline float countdownF(float x) {
		return 1 - 0.7 / (1 + powf(1.01f, 220 - x)) - 0.15 / (1 + powf(1.01f, 120 - 0.5f * x));
	};
};

