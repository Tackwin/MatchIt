#pragma once
#include <SFML\Graphics\RenderWindow.hpp>

class InputManager {
public:
	static bool keyJustPressed[];
	static bool mouseJustPressed[];

	InputManager();
	~InputManager();

	static void update(sf::RenderWindow &window);
};

