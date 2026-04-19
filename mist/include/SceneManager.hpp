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
		
		inline const int32_t GetActiveSceneIndex() { return activeScene; }
		inline entt::registry& GetActiveScene() { return loadedScenes[activeScene]; }

		inline void SubmitActiveScene() { SubmitScene(activeScene); }
		void SubmitScene(int32_t index);

		void UpdateSceneCamera();

		void LoadEmptyScene();
		void LoadScene();
		void SetActiveScene(int32_t index);

		void Cleanup();
	private:
		int32_t activeScene = -1;
		std::vector<entt::registry> loadedScenes;
	};
}