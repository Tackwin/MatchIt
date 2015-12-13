#pragma once
#include <vector>
#include <functional>
#include <entityx\entityx.h>
#include <SFML\System\Clock.hpp>

typedef std::function<bool(entityx::Entity &E)> callback;

//////////
/// function timer (0.f -> every frame)
//////////
struct ScriptInfo {
	callback script;
	sf::Clock internalClock;
	float priority = 0.f;
	float timer = 0.f;

	ScriptInfo() {}
	ScriptInfo(callback f, float priority = 0.f, float timer = 0.f) 
		: script(f), priority(priority), timer(timer) {}
};

struct Script {
	std::vector<ScriptInfo> scripts;

	static void add(entityx::Entity &e, const ScriptInfo &info) {
		if(!e.has_component<Script>()) e.assign<Script>();
		e.component<Script>()->scripts.push_back(info);
	}
};