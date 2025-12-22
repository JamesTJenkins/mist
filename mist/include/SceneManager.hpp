#pragma once
#include <entt/entt.hpp>

namespace mist {
	class SceneManager {
	public:
		const entt::entity CreateEntity();
		void DestroyEntity(const entt::entity entity);

		template<typename T, typename ... Args>
		T& AddComponent(Args&& ... args) {
			return loadedScenes[activeScene].emplace<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent(const entt::entity entity) {
			loadedScenes[activeScene].erase<T>(entity);
		}
		
		inline const uint32_t GetActiveSceneIndex() { return activeScene; }
		inline entt::registry& GetActiveScene() { return loadedScenes[activeScene]; }

		inline void SubmitActiveScene() { SubmitScene(activeScene); }
		void SubmitScene(uint32_t index);

		void LoadEmptyScene();
		void LoadScene();
		void SetActiveScene(uint32_t index) { activeScene = index; }

		void Cleanup();
	private:
		uint32_t activeScene = 0;
		std::vector<entt::registry> loadedScenes;
	};
}