#include "Game.hpp"

int main() {
	srand(time(NULL));

	Game::create();
	while(Game::window.isOpen()) {
		Game::update();
	}
	return 0;
}