#include "components/MeshRenderer.hpp"
#include "Application.hpp"

namespace mist {
	MeshRenderer::MeshRenderer() {}

	MeshRenderer::MeshRenderer(std::string shaderName, Mesh* mesh) : shaderName(shaderName), mesh(mesh) {
		Apply();
	}

	MeshRenderer::~MeshRenderer() {
		Clear();
	}

	void MeshRenderer::Bind() {
		Application::Get().GetRenderAPI()->BindMeshRenderer(*this);
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