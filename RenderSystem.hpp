#pragma once
#include "Comp\Graphics.hpp"

class RenderSystem : public entityx::System<RenderSystem> {
private:
	std::vector<SpriteInfo> sprites;
	std::vector<TextInfo> texts;

	entityx::ComponentHandle<Graphics> graphics;

	sf::RenderWindow &window;
public:
	RenderSystem(sf::RenderWindow &window);
	~RenderSystem();

	void update(entityx::EntityManager &enm, entityx::EventManager &evm, entityx::TimeDelta delta);
};

