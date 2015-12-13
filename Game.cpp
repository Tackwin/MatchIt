#include "Game.hpp"
#include "RenderSystem.hpp"
#include "Comp\Physics.hpp"
#include "Comp\Position.hpp"
#include "InputManager.hpp"
#include "ResMng.hpp"
#include "Destroyer.hpp"
#include "ScriptSystem.hpp"
#include "ItemSystem.hpp"
#include "SoundMng.hpp"
#include "Item.hpp"

sf::RenderWindow Game::window(sf::VideoMode(1280, 720), "Match It");
entityx::EntityX Game::entx;

sf::Clock Game::shakeClock;
sf::Clock Game::deltaClock;
sf::Clock Game::elapsedClock;
sf::Clock Game::countdownClock;
float Game::DELTA;
float Game::life = 100;
int Game::score = 0;
int Game::scoreMutliplier = 1;
int Game::bestCombo = 1;

sf::View Game::view;

std::default_random_engine Game::randGenrator;
std::uniform_int_distribution<int> Game::colorRandomizer;

sf::Color Game::colorRight;
sf::Color Game::colorLeft;

sf::Color Game::nextColorLeft;
sf::Color Game::nextColorRight;

entityx::Entity Game::cam;

Game::Game() {

}

Game::~Game() {
}

void Game::update() {
	DELTA = deltaClock.restart().asSeconds();

	entx.systems.update<ItemSystem>(0.0f);
	entx.systems.update<ScriptSystem>(0.0f);

	life = (life >= 0) ? life : 0;
	life = (life <= 100) ? life : 100;
	entx.systems.update<RenderSystem>(0.0f);

	InputManager::update(window);
	
	bestCombo = (scoreMutliplier > bestCombo) ? scoreMutliplier : bestCombo;

	SoundMng::update();
	Destroyer::update(entx.entities);
}

void Game::create() {
	ResMng::loadAll();

	ResMng::getMusic(IDLE).setVolume(20);
	ResMng::getMusic(IDLE).setLoop(true);
	ResMng::getMusic(IDLE).play();

	window.setVerticalSyncEnabled(false);
	view = sf::View(sf::FloatRect(0, 0, 1280, 720));

	randGenrator = std::default_random_engine(time(NULL));
	colorRandomizer = std::uniform_int_distribution<int>(80, 120);
	colorRight = sf::Color(colorRandomizer(randGenrator), colorRandomizer(randGenrator), colorRandomizer(randGenrator));
	colorLeft = sf::Color(255 - colorRight.r, 255 - colorRight.g, 255 - colorRight.b);
	if(colorRight.r * colorRight.g * colorRight.b  < colorLeft.r * colorLeft.g * colorLeft.b) {
		auto c = colorRight;
		colorRight = colorLeft;
		colorLeft = c;
	}

	nextColorRight = sf::Color(colorRandomizer(randGenrator), colorRandomizer(randGenrator), colorRandomizer(randGenrator));
	nextColorLeft = sf::Color(255 - nextColorRight.r, 255 - nextColorRight.g, 255 - nextColorRight.b);
	if(nextColorRight.r * nextColorRight.g * nextColorRight.b  < nextColorLeft.r * nextColorLeft.g * nextColorLeft.b) {
		auto c = nextColorRight;
		nextColorRight = nextColorLeft;
		nextColorLeft = c;
	}

	entx.systems.add<RenderSystem>(window);
	entx.systems.add<ScriptSystem>();
	entx.systems.add<ItemSystem>();
	entx.systems.configure();
	 
	entityx::Entity menu = entx.entities.create();
	Graphics::set(menu, "a", SpriteInfo(ResMng::getTexture(MENU)));
	Script::add(menu, ScriptInfo([](entityx::Entity &E) -> bool {
		Vector m(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
		if(InputManager::keyJustPressed[sf::Keyboard::Return] || (InputManager::mouseJustPressed[sf::Mouse::Left] && m.x >= 602 && m.x <= 647 && m.y >= 326 && m.y <= 349)) {
			entityx::Entity wait = entx.entities.create();
			Graphics::set(wait, "A", SpriteInfo(ResMng::getTexture(WAITING)));
			Graphics::set(wait, "B", TextInfo(sf::Text("3", ResMng::getFont(FONT)), Vector(615, 100)));
			sf::Clock *c = new sf::Clock();
			Script::add(wait, ScriptInfo([c](entityx::Entity &E) -> bool {
				bool flag = false;
				for(int i = 0; i < sf::Keyboard::KeyCount; i++) flag = (InputManager::keyJustPressed[i]) ? true : flag;
				for(int i = 0; i < sf::Mouse::ButtonCount; i++) flag = (InputManager::mouseJustPressed[i]) ? true : flag;
				if(flag) {
					Destroyer::destroying.push_back(E);
					delete c;
					launchGame();
					return true;
				}
				if(c->getElapsedTime().asSeconds() >= 1) {
					sf::Text &t = E.component<Graphics>()->texts["B"].text;
					t.setString(std::to_string(std::atoi(t.getString().toAnsiString().c_str()) - 1));
					c->restart();
				}
				return false;
			}));
			Script::add(wait, ScriptInfo([c](entityx::Entity &E) -> bool {
				Destroyer::destroying.push_back(E);
				delete c;
				launchGame();
				return true;
			}, 0.f, 3.f));
			Destroyer::destroying.push_back(E);
		}
		return false;
	}));

	//launchGame();
}

void Game::launchGame() {
	ResMng::getMusic(MUSIC).setVolume(0);
	ResMng::getMusic(MUSIC).setLoop(true);
	ResMng::getMusic(MUSIC).play();
	entityx::Entity fadeIdle = entx.entities.create();
	Script::add(fadeIdle, ScriptInfo([](entityx::Entity &E) -> bool {
		ResMng::getMusic(IDLE).setVolume(ResMng::getMusic(IDLE).getVolume() - DELTA * 40);
		return false;
	}));
	Script::add(fadeIdle, ScriptInfo([](entityx::Entity &E) -> bool {
		ResMng::getMusic(IDLE).stop();
		Destroyer::destroying.push_back(E);
		return true;
	}, 0.f, 0.5f));
	entityx::Entity fadeMusic = entx.entities.create();
	Script::add(fadeMusic, ScriptInfo([](entityx::Entity &E) -> bool {
		ResMng::getMusic(MUSIC).setVolume(ResMng::getMusic(MUSIC).getVolume() + DELTA * 60);
		return false;
	}));
	Script::add(fadeMusic, ScriptInfo([](entityx::Entity &E) -> bool {
		ResMng::getMusic(MUSIC).setVolume(30);
		Destroyer::destroying.push_back(E);
		return true;
	}, 0.f, 0.5f));

	entx.systems.system<ItemSystem>()->n = 0;
	life = 100;
	score = 0;
	scoreMutliplier = 1;
	bestCombo = 0;

	entityx::Entity e = entx.entities.create();
	Graphics::set(e, "firstHalf", SpriteInfo(ResMng::getTexture(BACKGORUNDHALF), Vector::ZERO, 0, colorLeft));
	Graphics::set(e, "secondHalf", SpriteInfo(ResMng::getTexture(BACKGORUNDHALF), Vector(640, 0), 0, colorRight));
	Graphics::set(e, "previewFirstHalf", SpriteInfo(ResMng::getTexture(PREVIEW), Vector(0, 9 * 720 / 10), 0.1f, nextColorLeft));
	Graphics::set(e, "previewSecondHalf", SpriteInfo(ResMng::getTexture(PREVIEW), Vector(9 * 1280 / 10, 9 * 720 / 10), 0.1f, nextColorRight));
	Script::add(e, ScriptInfo([](entityx::Entity &E) -> bool {
		E.component<Graphics>()->sprites["firstHalf"].color = colorLeft;
		E.component<Graphics>()->sprites["secondHalf"].color = colorRight;
		E.component<Graphics>()->sprites["previewFirstHalf"].color = nextColorLeft;
		E.component<Graphics>()->sprites["previewSecondHalf"].color = nextColorRight;
		if(life == 0) {
			Destroyer::destroying.push_back(E);
			return true;
		}
		return false;
	}));

	entityx::Entity ui = entx.entities.create();
	int i = 0;
#ifdef _DEBUG
	Graphics::set(ui, "FPS", TextInfo(sf::Text("Fps: ", ResMng::getFont(FONT), 14), Vector(10, 5))); i++;
#endif
	Graphics::set(ui, "Score", TextInfo(sf::Text("Score: ", ResMng::getFont(FONT), 14), Vector(10, 5 + 15 * i))); i++;
	Graphics::set(ui, "ScoreMultiplier", TextInfo(sf::Text("Combo: ", ResMng::getFont(FONT), 14), Vector(10, 5 + 15 * i))); i++;
	Graphics::set(ui, "HoverBarLife", SpriteInfo(ResMng::getTexture(HOVERBARLIFE), Vector(1160, 10), 0.1f));
	Graphics::set(ui, "HoverBarCountdown", SpriteInfo(ResMng::getTexture(HOVERBARCOUNTDOWN), Vector(140, 10), 0.1f));
	Graphics::set(ui, "FullBarCountdown", SpriteInfo(ResMng::getTexture(FULLBARCOUNTDOWN), Vector(140, 10), 0.05f));
	Graphics::set(ui, "FullBarLife", SpriteInfo(ResMng::getTexture(FULLBARLIFE), Vector(1163, 10), 0.05f));
	Script::add(ui, ScriptInfo([](entityx::Entity &E) -> bool {
#ifdef _DEBUG
		sf::Text &fps = E.component<Graphics>()->texts["FPS"].text;
		fps.setString("FPS: " + std::to_string(static_cast<int>(1 / Game::DELTA)));
#endif

		sf::Sprite &fullBarLife = E.component<Graphics>()->sprites["FullBarLife"].sprite;
		fullBarLife.setTextureRect(sf::IntRect(0, 0, life * 1.04f, 30));

		sf::Sprite &fullBarCountdown = E.component<Graphics>()->sprites["FullBarCountdown"].sprite;
		float progress = countdownClock.getElapsedTime().asSeconds() / countdownF(entx.systems.system<ItemSystem>()->n);
		progress = (progress >= 0) ? progress : 0;
		progress = (progress <= 1) ? progress : 1;
		fullBarCountdown.setTextureRect(sf::IntRect(0, 0, 1010 * (1 - progress), 50));

		sf::Text &score = E.component<Graphics>()->texts["Score"].text;
		score.setString("Score: " + std::to_string(Game::score));	
		
		sf::Text &mult = E.component<Graphics>()->texts["ScoreMultiplier"].text;
		mult.setString("Combo: " + std::to_string(Game::scoreMutliplier));

		if(life == 0) {
			Destroyer::destroying.push_back(E);
			return true;
		}
		return false;
	}));

	entityx::Entity colorChanger = entx.entities.create();
	sf::Clock *c = new sf::Clock();
	Script::add(colorChanger, ScriptInfo([c](entityx::Entity &E) {
		if(c->getElapsedTime().asSeconds() >= 30 * countdownF(entx.systems.system<ItemSystem>()->n)) {
			colorLeft = nextColorLeft;
			colorRight = nextColorRight;

			nextColorRight = sf::Color(colorRandomizer(randGenrator), colorRandomizer(randGenrator), colorRandomizer(randGenrator));
			nextColorLeft = sf::Color(255 - nextColorRight.r, 255 - nextColorRight.g, 255 - nextColorRight.b);
			if(nextColorRight.r * nextColorRight.g * nextColorRight.b  < nextColorLeft.r * nextColorLeft.g * nextColorLeft.b) {
				auto c = nextColorRight;
				nextColorRight = nextColorLeft;
				nextColorLeft = c;
			}
			c->restart();
		}
		if(life == 0) {
			Destroyer::destroying.push_back(E);
			return true;
		}
		return false;
	}, 0.f, 1.f));

	for(int i = 0; i < Item::WAINTINGMAX; i++) {
		entx.systems.system<ItemSystem>()->addItem();
	}

	entityx::Entity gameOver = entx.entities.create();
	Graphics::set(gameOver, "OverScreen", SpriteInfo(ResMng::getTexture(GAMEOVER), Vector::ZERO, 0.f, sf::Color::Transparent));
	Graphics::set(gameOver, "ScoreFinal", TextInfo(sf::Text("     Score: " + std::to_string(score) +"\nrestart by pressing anything", ResMng::getFont(FONT)), Vector(450, 300), 0.1f, sf::Color::Transparent));
	Script::add(gameOver, ScriptInfo([](entityx::Entity &E) -> bool {
		if(life == 0) {
			for(int i = 0; i < entx.systems.system<ItemSystem>()->items.size(); i++) {
				Destroyer::destroying.push_back(entx.systems.system<ItemSystem>()->items[i]);
			}
			entx.systems.system<ItemSystem>()->items.clear();
			entityx::Entity fadeMusic = entx.entities.create();
			Script::add(fadeMusic, ScriptInfo([](entityx::Entity &E) -> bool {
				ResMng::getMusic(MUSIC).setVolume(ResMng::getMusic(MUSIC).getVolume() - DELTA * 60);
				return false;
			}));
			Script::add(fadeMusic, ScriptInfo([](entityx::Entity &E) -> bool {
				ResMng::getMusic(MUSIC).stop();
				Destroyer::destroying.push_back(E);
				return true;
			}, 0.f, 0.5f));
			ResMng::getMusic(IDLE).play();
			ResMng::getMusic(IDLE).setVolume(0);
			entityx::Entity fadeIdle = entx.entities.create();
			Script::add(fadeIdle, ScriptInfo([](entityx::Entity &E) -> bool {
				ResMng::getMusic(IDLE).setVolume(ResMng::getMusic(IDLE).getVolume() + DELTA * 40);
				return false;
			}));
			Script::add(fadeIdle, ScriptInfo([](entityx::Entity &E) -> bool {
				ResMng::getMusic(IDLE).setVolume(20);
				Destroyer::destroying.push_back(E);
				return true;
			}, 0.f, 0.5f));

			E.component<Graphics>()->texts["ScoreFinal"].color = colorRight; 
			E.component<Graphics>()->texts["ScoreFinal"].text.setString("                Score: " + std::to_string(score) + "\n            Best combo: " + std::to_string(bestCombo) + "\n restart by pressing anything");
			E.component<Graphics>()->sprites["OverScreen"].color = colorLeft;
			return true;
		}
		return false;
	}));
	Script::add(gameOver, ScriptInfo([](entityx::Entity &E) -> bool {
		if(life == 0) {
			bool flag = false;
			for(int i = 0; i < sf::Keyboard::KeyCount; i++) flag = (InputManager::keyJustPressed[i]) ? true : flag;
			for(int i = 0; i < sf::Mouse::ButtonCount; i++) flag = (InputManager::mouseJustPressed[i]) ? true : flag;
			if(flag) {
				launchGame();
				colorLeft = nextColorLeft;
				colorRight = nextColorRight;

				nextColorRight = sf::Color(colorRandomizer(randGenrator), colorRandomizer(randGenrator), colorRandomizer(randGenrator));
				nextColorLeft = sf::Color(255 - nextColorRight.r, 255 - nextColorRight.g, 255 - nextColorRight.b);
				if(nextColorRight.r * nextColorRight.g * nextColorRight.b  < nextColorLeft.r * nextColorLeft.g * nextColorLeft.b) {
					auto c = nextColorRight;
					nextColorRight = nextColorLeft;
					nextColorLeft = c;
				}
				Destroyer::destroying.push_back(E);
				return true;
			}
		}
		return false;
	}));
}

void Game::shakeCam() {
	if(cam)
		cam.destroy();
	shakeClock.restart();
	view.setSize(Vector(1280, 720));
	cam = entx.entities.create();
	Script::add(cam, ScriptInfo([](entityx::Entity &E) {
		float x = shakeClock.getElapsedTime().asSeconds();
		view.setSize(Vector(1280, 720) * (1 - 0.1f / (1 + expf(1000 * (x - 0.08f) * (x - 0.08f)))));
		if(x >= 0.2f) {
			view.setSize(Vector(1280, 720));
			return true;
		}
		return false;
	}));
}