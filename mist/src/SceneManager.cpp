#include "SceneManager.hpp"
#include "components/Transform.hpp"

namespace mist {
	const entt::entity SceneManager::CreateEntity() {
		const entt::entity entity = currentScene.create();
		return entity;
	}

	void SceneManager::DestroyEntity(const entt::entity entity) {
		currentScene.destroy(entity);
	}
}