#include "components/MeshRenderer.hpp"
#include "Application.hpp"

namespace mist {
	MeshRenderer::MeshRenderer(Transform& transform, std::string shaderName, mist::Ref<Mesh> mesh) : transformComponent(transform), shaderName(shaderName), mesh(mesh) {
		Apply();
	}

	MeshRenderer::~MeshRenderer() {
		Clear();
	}

	void MeshRenderer::Bind() {
		Application::Get().GetRenderAPI()->BindMeshRenderer(*this);
	}
	
	void MeshRenderer::Draw() {
		Application::Get().GetRenderAPI()->Draw(mesh->indices.size());
	}

	void MeshRenderer::Apply() {
		Clear();
		vBuffer = mist::VertexBuffer::Create(mesh->vertices);
		iBuffer = mist::IndexBuffer::Create(mesh->indices);
	}

	void MeshRenderer::Clear() {
		if (vBuffer != nullptr)
			vBuffer->Clear();

		if (iBuffer != nullptr)
			iBuffer->Clear();
	}
}