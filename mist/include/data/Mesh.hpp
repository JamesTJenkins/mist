#pragma once
#include <vector>
#include "Math.hpp"

namespace mist {
    struct Vertex {
		glm::vec3 position;
        glm::vec3 normal;
	};

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Mesh();
        Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices);

        void GenerateNormals();
    };
}