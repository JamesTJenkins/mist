#pragma once
#include <entt/entt.hpp>

namespace mist {
	class SceneManager {
	public:
		const entt::entity CreateEntity();
		void DestroyEntity(const entt::entity entity);

		entt::registry currentScene;
	};
}