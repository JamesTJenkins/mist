#pragma once
#include <vector>
#include "Math.hpp"

namespace mist {
    struct Vertex {
		glm::vec3 position;
	};

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Mesh() {}
        Mesh(std::vector<Vertex> _vertices, std::vector<uint32_t> _indices) {
            vertices = _vertices;
            indices = _indices;
        }
    };
}