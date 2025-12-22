#include "SceneManager.hpp"
#include "components/Transform.hpp"
#include <Application.hpp>
#include <Debug.hpp>

namespace mist {
	const entt::entity SceneManager::CreateEntity() {
		const entt::entity entity = loadedScenes[activeScene].create();
		return entity;
	}

	void SceneManager::DestroyEntity(const entt::entity entity) {
		loadedScenes[activeScene].destroy(entity);
	}

	void SceneManager::SubmitScene(uint32_t sceneIndex) {
		ShaderLibrary* shaderLib = Application::Get().GetShaderLibrary();
		auto view = loadedScenes[sceneIndex].view<MeshRenderer>();
		
		// Binding and unbinding a shader pipeline after each object is terrible but will do for testing sake
		// ideally we bind a shader then render everything with that shader before moving on
		// unless there is better methods im unaware of
		std::string currentPipeline;
		view.each([shaderLib, &currentPipeline](MeshRenderer &renderer) {
			if (renderer.shaderName.compare(currentPipeline) != 0) {
				shaderLib->Get(renderer.shaderName)->Bind();
				currentPipeline = renderer.shaderName;
			}
			
			renderer.Bind();
			renderer.Draw();
		});
	}

	void SceneManager::UpdateSceneCamera() {
		auto camView = loadedScenes[activeScene].view<Camera>();
		
		Camera* cam;
		for (auto entity : camView) {
			cam = loadedScenes[activeScene].try_get<Camera>(entity);

			if (cam != nullptr)
				break;
		}

		if (cam != nullptr)
			Application::Get().GetRenderAPI()->UpdateCamera(*cam);
	}

	void SceneManager::LoadEmptyScene() {
		loadedScenes.push_back(entt::registry{});
		MIST_INFO("Loaded empty scene");
	}

	void SceneManager::LoadScene() {
		MIST_WARN("Currently not implemented");
	}

	void SceneManager::Cleanup() {
		for (size_t i = 0; i < loadedScenes.size(); ++i) {
			auto view = loadedScenes[i].view<MeshRenderer>();
			view.each([](MeshRenderer &renderer) {
				renderer.Clear();
			});
			
			loadedScenes[i].clear();
		}
	}
}