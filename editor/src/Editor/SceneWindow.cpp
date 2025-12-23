#include "SceneWindow.hpp"
#include <renderer/Buffer.hpp>
#include <vector>
#include <Application.hpp>
#include <components/MeshRenderer.hpp>
#include <components/Collider.hpp>
#include <components/Rigidbody.hpp>

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

		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->LoadEmptyScene();
		
		std::vector<mist::Vertex> verts = {
			{{ -1.0f, -1.0f, 0.0f }},
			{{ -1.0f,  1.0f, 0.0f }},
			{{  1.0f, -1.0f, 0.0f }},
			{{  1.0f,  1.0f, 0.0f }}
		};
		
		std::vector<uint32_t> indices = {
			0, 1, 2,
			1, 3, 2
		};
		
		testMesh = mist::CreateRef<mist::Mesh>(verts, indices);
		testShader = mist::Application::Get().GetShaderLibrary()->Load("assets/test.glsl");
		
		{
			const entt::entity triEntity = sm->CreateEntity();
			mist::Transform& testT = sm->AddComponent<mist::Transform>(triEntity, glm::vec3(2, -0.5, 0));
			sm->AddComponent<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMesh);
			sm->AddComponent<mist::Rigidbody>(triEntity, 1.0f, 0.5f, glm::vec3(-1, 0, 0));
			sm->AddComponent<mist::Collider>(triEntity, mist::SphereCollider(1.0f));
		}

		{
			const entt::entity triEntity = sm->CreateEntity();
			mist::Transform& testT = sm->AddComponent<mist::Transform>(triEntity, glm::vec3(-2, 0.5, 0));
			sm->AddComponent<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMesh);
			sm->AddComponent<mist::Rigidbody>(triEntity, 1.0f, 0.5f, glm::vec3(1, 0, 0));
			sm->AddComponent<mist::Collider>(triEntity, mist::SphereCollider(1.0f));
		}

		const entt::entity sceneCameraEntity = sm->CreateEntity();
		mist::Transform& sceneCameraT = sm->AddComponent<mist::Transform>(sceneCameraEntity, glm::vec3(0, 0, -10));
		mist::Camera& sceneCamera = sm->AddComponent<mist::Camera>(sceneCameraEntity, sceneCameraT);
		sceneCamera.SetPerspectiveCamera(1280, 720);
	}

	void SceneWindow::Cleanup() {
		testShader->Clear();
	}

	void SceneWindow::OnEditorUpdate() {
		mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
	}

	void SceneWindow::OnRender() {
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->UpdateSceneCamera();
		sm->SubmitActiveScene();
	}
}