#pragma once
#include <string>
#include "data/Mesh.hpp"
#include "renderer/Buffer.hpp"
#include "components/Transform.hpp"

namespace mist {
    class MeshRenderer {
    public:
        MeshRenderer(Transform& transform, std::string shaderName, Mesh* mesh = nullptr);
        ~MeshRenderer();

        void Bind();
        void Draw();
        void Apply();
        void Clear();

        inline void SetTransform(Transform& value) { transformComponent = value; }
		inline Transform& GetTransform() const { return transformComponent; }

        Mesh* mesh;
        std::string shaderName; // TODO: this will be changed when doing materials properly
        mist::Ref<VertexBuffer> vBuffer;
        mist::Ref<IndexBuffer> iBuffer;
    private:
        Transform& transformComponent;
    };
}