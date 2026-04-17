#include "SceneWindow.hpp"
#include <renderer/Buffer.hpp>
#include <vector>
#include <Application.hpp>
#include <components/MeshRenderer.hpp>
#include <components/Collider.hpp>
#include <components/Rigidbody.hpp>
#include <components/DirectionalLight.hpp>
#include <data/Importer.hpp>

namespace mistEditor {
	SceneWindow::SceneWindow() {}

	void SceneWindow::Initialize() {
		std::vector<mist::FramebufferTextureProperties> attachments = {
			mist::FramebufferTextureFormat::RGBA8,
			mist::FramebufferTextureFormat::DEPTH32_STENCIL8
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
		
		testShader = mist::Application::Get().GetShaderLibrary()->Load("assets/lambert.glsl");

		//testMeshes = mist::Importer::ImportMeshes("assets/brassfang.fbx");
		testMeshes = mist::Importer::ImportMeshes("assets/LightCycle.obj", true);
		{
			const entt::entity triEntity = sm->CreateEntity();
			mist::Transform& testT = sm->AddComponent<mist::Transform>(triEntity, glm::vec3(-2, 0, 0), glm::quat_identity<float, glm::defaultp>(), glm::vec3(1.0f));
			sm->AddComponent<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMeshes[0]);
		}

		std::vector<mist::Vertex> verts = {
			{{ -1.0f, -1.0f, 0.0f }, {0,0,0}},
			{{ -1.0f,  1.0f, 0.0f }, {0,0,0}},
			{{  1.0f, -1.0f, 0.0f }, {0,0,0}},
			{{  1.0f,  1.0f, 0.0f }, {0,0,0}}
		};
		
		std::vector<uint32_t> indices = {
			0, 1, 2,
			3, 2, 1
		};
		
		mist::Ref<mist::Mesh> testMesh = mist::CreateRef<mist::Mesh>(verts, indices);
		testMesh->GenerateNormals();
		{
			const entt::entity triEntity = sm->CreateEntity();
			mist::Transform& testT = sm->AddComponent<mist::Transform>(triEntity, glm::vec3(2, 0, 0));
			sm->AddComponent<mist::MeshRenderer>(triEntity, testT, testShader->GetName(), testMesh);
		}

		const entt::entity sceneCameraEntity = sm->CreateEntity();
		mist::Transform& sceneCameraT = sm->AddComponent<mist::Transform>(sceneCameraEntity, glm::vec3(0, 0, -5));
		mist::Camera& sceneCamera = sm->AddComponent<mist::Camera>(sceneCameraEntity, sceneCameraT);
		sceneCamera.SetPerspectiveCamera(1280, 720);

		const entt::entity directionalLightEntity = sm->CreateEntity();
		mist::Transform& directionalLightT = sm->AddComponent<mist::Transform>(directionalLightEntity, glm::vec3(0, 0, -5), glm::quat(glm::radians(glm::vec3(-45, 180, 0))));
		mist::DirectionalLight& directionalLight = sm->AddComponent<mist::DirectionalLight>(directionalLightEntity, directionalLightT, glm::vec3(1,1,1));
	}

	void SceneWindow::Cleanup() {
		testShader->Clear();
	}

	void SceneWindow::OnEditorUpdate() {
		mist::RenderAPI* renderAPI = mist::Application::Get().GetRenderAPI();
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		float delta = mist::Application::Get().GetDeltaTime();

		auto view = sm->GetActiveScene().view<mist::Transform, mist::MeshRenderer>();
		view.each([delta](mist::Transform &transform, mist::MeshRenderer &render) {
			transform.Rotate(glm::radians(30.0f) * delta, { 0, 1, 0 });
		});
	}

	void SceneWindow::OnRender() {
		mist::SceneManager* sm = mist::Application::Get().GetSceneManager();
		sm->UpdateSceneCamera();
		sm->SubmitActiveScene();
	}
}