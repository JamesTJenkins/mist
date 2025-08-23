#pragma once
#include <string>
#include "data/Mesh.hpp"
#include "renderer/Buffer.hpp"

namespace mist {
    class MeshRenderer {
    public:
        MeshRenderer();
        MeshRenderer(std::string shaderName, Mesh* mesh = nullptr);
        ~MeshRenderer();

        void Bind();
        void Apply();
        void Clear();

        Mesh* mesh;
        std::string shaderName; // TODO: this will be changed when doing materials properly
        mist::Ref<VertexBuffer> vBuffer;
        mist::Ref<IndexBuffer> iBuffer;
    };
}