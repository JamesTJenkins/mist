#pragma once
#include <string>
#include "data/Mesh.hpp"
#include "renderer/Buffer.hpp"
#include "components/Transform.hpp"

namespace mist {
    class MeshRenderer {
    public:
        MeshRenderer(Transform& transform, std::string shaderName, Ref<Mesh> mesh);
        ~MeshRenderer();

        void Bind();
        void Draw();
        void Apply();
        void Clear();

        inline void SetTransform(Transform& value) { transformComponent = value; }
		inline Transform& GetTransform() const { return transformComponent; }

        std::string shaderName; // TODO: this will be changed when doing materials properly
        Ref<Mesh> mesh;
        Ref<VertexBuffer> vBuffer;
        Ref<IndexBuffer> iBuffer;
    private:
        Transform& transformComponent;
    };
}