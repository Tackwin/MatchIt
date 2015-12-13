#pragma once
#include <entityx\entityx.h>

struct Destroyer {
	static std::vector<entityx::Entity> destroying;

	static void update(entityx::EntityManager &enm) {
		for(auto &A : destroying) {
			if(A)
				enm.destroy(A.id());
		}
		destroying.swap(std::vector<entityx::Entity>());
	}
};
