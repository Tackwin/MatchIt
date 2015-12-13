#include "InputManager.hpp"
#include <SFML\Window\Keyboard.hpp>
#include <SFML\Window\Mouse.hpp>
#include <SFML\Window\Event.hpp>

bool InputManager::keyJustPressed[sf::Keyboard::KeyCount];
bool InputManager::mouseJustPressed[sf::Mouse::ButtonCount];

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::update(sf::RenderWindow &window) {
	for(auto &b : keyJustPressed) b = false;
	for(auto &b : mouseJustPressed) b = false;

	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
			window.close();
		}
		if(event.type == sf::Event::KeyPressed) {
			keyJustPressed[event.key.code] = true;
		}
		if(event.type == sf::Event::MouseButtonPressed) {
			mouseJustPressed[event.mouseButton.button] = true;
		}
	}

}