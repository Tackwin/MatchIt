#include "ItemSystem.hpp"
#include <SFML\Graphics.hpp>
#include "Script.hpp"
#include "Comp\Position.hpp"
#include "Game.hpp"
#include "Comp\Graphics.hpp"
#include "Destroyer.hpp"
#include "ResMng.hpp"
#include "InputManager.hpp"
#include "Item.hpp"
#include "SoundMng.hpp"

ItemSystem::ItemSystem() {
}

ItemSystem::~ItemSystem() {
}

bool sortItem(entityx::Entity &A, entityx::Entity &B) {
	return A.component<Item>()->waitingPos < B.component<Item>()->waitingPos;
}

void ItemSystem::update(entityx::EntityManager &enm, entityx::EventManager &evm, entityx::TimeDelta delta) {
	bool shift = false;

	for(int i = 0; i < items.size(); i++) {
		if(!items[i]) {
			items.erase(items.begin() + i);
			i--;
		}
	}
	std::sort(items.begin(), items.end(), sortItem);
	for(int i = 0; i < items.size(); i++) {
		if(items[i].component<Item>()->waitingPos != 0 && i == 0) {
			Script::add(items[i], ScriptInfo([this](entityx::Entity &E) -> bool {
				if(!E.has_component<Item>()) return true;
				items.erase(items.begin());
				addItem();
				launchFailAnim();
				Destroyer::destroying.push_back(E);
				return true;
			}, 0, Game::countdownF(n)));
		}
		items[i].component<Item>()->waitingPos = i;
	}

	for(int i = 0; i < items.size(); i++) {
		sf::Color &c = items[i].component<Graphics>()->sprites["item"].color;
		switch(items[i].component<Item>()->target) {
		case Item::LEFT:
			c = Game::colorLeft;
			break;
		case Item::RIGHT:
			c = Game::colorRight;
			break;
		default:
			c = sf::Color::Black;
		}
		c *= sf::Color(200, 200, 200);
		Vector &p = items[i].component<Position>()->get();
		Vector d = Vector(640, 240 + 90 * i);
		if(p.distTo2(d) > 25) {
			p += (d - p) * 30 * Game::DELTA;
		}
	}
}

void ItemSystem::addItem() {
	Game::countdownClock.restart();

	std::uniform_int_distribution<int> dist(0, 3000);

	entityx::Entity e = Game::entx.entities.create();
	e.assign<Position>(640, 240 + 90 * (items.size() + 1));
	e.assign<Item>();
	e.component<Item>()->waitingPos = items.size();
	int r = dist(Game::randGenrator);
	if(r <= 1490) {
		r = Item::LEFT;
	} else if(r <= 2980) {
		r = Item::RIGHT;
	} else {
		r = Item::EITHER;
	}
	e.component<Item>()->target = r;
	sf::Color color;
	switch(r) {
	case Item::LEFT:
		color = Game::colorLeft;
		break;
	case Item::RIGHT:
		color = Game::colorRight;
		break;
	default:
		color = sf::Color::Black;
	}
	Graphics::set(e, "item", SpriteInfo((r != Item::EITHER) ? ResMng::getTexture(ITEM) : ResMng::getTexture(ACTION), Vector(-40, -40), 1.f, color * sf::Color(200, 200, 200)));
	Script::add(e, ScriptInfo([this](entityx::Entity &E) -> bool {
		if(E.component<Item>()->waitingPos == 0) {
			if(InputManager::keyJustPressed[sf::Keyboard::D] || InputManager::keyJustPressed[sf::Keyboard::Right] || InputManager::mouseJustPressed[sf::Mouse::Right]) {
				items.erase(items.begin());
				addItem();
				int t = E.component<Item>()->target;
				if(t == Item::LEFT) {
					launchFailAnim();
					Game::scoreMutliplier = 1;
					E.remove<Item>();
				} else if(t != Item::EITHER){
					Game::score += Game::scoreMutliplier;
					Game::scoreMutliplier += 1;
					Game::shakeCam();
					E.remove<Item>();
				} else {
					Game::score += 100;
					Game::life += 20;
				}
				Script::add(E, ScriptInfo([](entityx::Entity &A) {
					if(Game::life == 0) Destroyer::destroying.push_back(A);
					if((A.component<Position>()->get() += Vector(1065, 100) * Game::DELTA).x >= 3 * 1280 / 4) {
						Destroyer::destroying.push_back(A);
						return true;
					}
					return false;
				}));
				return true;
			}
			if(InputManager::keyJustPressed[sf::Keyboard::Q] || InputManager::keyJustPressed[sf::Keyboard::W] || InputManager::keyJustPressed[sf::Keyboard::Left] || InputManager::mouseJustPressed[sf::Mouse::Left]) {
				items.erase(items.begin());
				addItem();
				int t = E.component<Item>()->target;
				if(t == Item::RIGHT) {
					launchFailAnim();
					Game::scoreMutliplier = 1;
					E.remove<Item>();
				} else if (t != Item::EITHER) {
					Game::score += Game::scoreMutliplier;
					Game::scoreMutliplier += 1;
					Game::shakeCam();
					E.remove<Item>();
				} else {
					Game::score += 100;
					Game::life += 20;
				}
				Script::add(E, ScriptInfo([](entityx::Entity &A) {
					if(Game::life == 0) Destroyer::destroying.push_back(A);
					if((A.component<Position>()->get() += Vector(-1065, 100) * Game::DELTA).x <= 1280 / 4) {
						Destroyer::destroying.push_back(A);
						return true;
					}
					return false;
				}));
				return true;
			}
		}
		return false;
	}));
	if(r == Item::EITHER) {
		Script::add(e, ScriptInfo([](entityx::Entity &E) -> bool {
			if(E.component<Item>()->waitingPos == 0) {
				if(InputManager::keyJustPressed[sf::Keyboard::Q] || InputManager::keyJustPressed[sf::Keyboard::W] || InputManager::keyJustPressed[sf::Keyboard::Left] || InputManager::mouseJustPressed[sf::Mouse::Left] ||
					InputManager::keyJustPressed[sf::Keyboard::D] || InputManager::keyJustPressed[sf::Keyboard::Right] || InputManager::mouseJustPressed[sf::Mouse::Right]) {
					Game::colorLeft = Game::nextColorLeft;
					Game::colorRight = Game::nextColorRight;

					Game::nextColorRight = sf::Color(Game::colorRandomizer(Game::randGenrator), Game::colorRandomizer(Game::randGenrator), Game::colorRandomizer(Game::randGenrator));
					Game::nextColorLeft = sf::Color(255 - Game::nextColorRight.r, 255 - Game::nextColorRight.g, 255 - Game::nextColorRight.b);
					if(Game::nextColorRight.r * Game::nextColorRight.g * Game::nextColorRight.b  < Game::nextColorLeft.r * Game::nextColorLeft.g * Game::nextColorLeft.b) {
						auto c = Game::nextColorRight;
						Game::nextColorRight = Game::nextColorLeft;
						Game::nextColorLeft = c;
					}
					return true;
				}
			}
			return false;
		}));
	}
	items.push_back(e);
}

void ItemSystem::launchFailAnim() {
	SoundMng::addSound(ResMng::getSound(FAILSONG), 20);
	Game::life -= 34.f;
	entityx::Entity e = Game::entx.entities.create();
	Graphics::set(e, "A", SpriteInfo(ResMng::getTexture(FAIL), Vector::ZERO, 10, sf::Color(255, 255, 255, 0)));
	sf::Clock *clock = new sf::Clock();
	Script::add(e, ScriptInfo([clock](entityx::Entity &E) -> bool {
		float x = clock->getElapsedTime().asSeconds();
		E.component<Graphics>()->sprites["A"].color = sf::Color(255, 255, 255, 255 * expf(-100 * (x - 0.2f) * (x - 0.2f)));
		if(x > 0.4f) {
			Destroyer::destroying.push_back(E);
			delete clock;
			return true;
		}
		return false;
	}));
}
