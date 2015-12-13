#pragma once
#include <entityx\entityx.h>

class ItemSystem : public entityx::System<ItemSystem> {
public:
	std::vector<entityx::Entity> items;

public:
	int n = 0;

	ItemSystem();
	~ItemSystem();

	void update(entityx::EntityManager &enm, entityx::EventManager &evm, entityx::TimeDelta delta);
	void addItem();
	void launchFailAnim();
};

