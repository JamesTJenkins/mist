#include "SceneWindow.hpp"
#include <renderer/Buffer.hpp>
#include <vector>
#include <components/MeshRenderer.hpp>
#include <Application.hpp>

namespace mistEditor {
	SceneWindow::SceneWindow() {}

	void SceneWindow::Initialize() {
		std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::FramebufferTextureFormat::RGBA8
		};
		mist::FramebufferProperties properties;
		properties.attachment.attachments = attachments.data();
		properties.attachment.attachmentsCount = (uint32_t)attachments.size();
		properties.width = 1280;
		properties.height = 720;
		mist::Framebuffer::Create(properties);
		mist::Application::Get().GetRenderAPI()->SetViewport(0, 0, 1280, 720);

		std::vector<mist::Vertex> verts = {
			{{  0.0f,  0.0f, 0.0f }},
			{{  0.0f,  1.0f, 0.0f }},
			{{  1.0f,  0.0f, 0.0f }},
			{{  1.0f,  1.0f, 0.0f }}
		};

		std::vector<uint32_t> indices = {
			0, 1, 2,
			1, 3, 2
		};

		testMesh = mist::CreateRef<mist::Mesh>(verts, indices);
		testShader = mist::Application::Get().GetShaderLibrary()->Load("assets/test.glsl");
		
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		const entt::entity triEntity = sm->CreateEntity();
		mist::Transform& testT = sm->currentScene.emplace<mist::Transform>(triEntity, glm::vec3(0, 0, 0));
		sm->currentScene.emplace<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMesh);
		
		const entt::entity sceneCameraEntity = sm->CreateEntity();
		mist::Transform& sceneCameraT = sm->currentScene.emplace<mist::Transform>(sceneCameraEntity, glm::vec3(0, .5, -2));
		mist::Camera& sceneCamera = sm->currentScene.emplace<mist::Camera>(sceneCameraEntity, sceneCameraT);
		sceneCamera.SetPerspectiveCamera(1280, 720);
	}

	void SceneWindow::Cleanup() {
		auto view = mist::Application::Get().GetSceneManager()->currentScene.view<mist::MeshRenderer>();
		view.each([](mist::MeshRenderer &renderer) {
			renderer.Clear();
		});

		mist::Application::Get().GetSceneManager()->currentScene.clear();
		testShader->Clear();
	}

	void SceneWindow::OnEditorUpdate() {
		mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		auto camView = sm->currentScene.view<mist::Camera>();

		mist::Camera* cam;
		for (auto entity : camView) {
			cam = sm->currentScene.try_get<mist::Camera>(entity);
			break;
		}
		renderAPI->UpdateCamera(*cam);
	}

	void SceneWindow::OnRender() {
		mist::ShaderLibrary* shaderLib = mist::Application::Get().GetShaderLibrary();
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		auto view = sm->currentScene.view<mist::MeshRenderer>();
		
		// Binding and unbinding a shader pipeline after each object is terrible but will do for testing sake
		// ideally we bind a shader then render everything with that shader before moving on
		// unless there is better methods im unaware of
		std::string currentPipeline;
		view.each([shaderLib, &currentPipeline](mist::MeshRenderer &renderer) {
			if (renderer.shaderName.compare(currentPipeline) != 0) {
				shaderLib->Get(renderer.shaderName)->Bind();
				currentPipeline = renderer.shaderName;
			}
			
			renderer.Bind();
			renderer.Draw();
		});
	}
}