#include "data/Mesh.hpp"

namespace mist {
	Mesh::Mesh() {}

	Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices) : vertices(_vertices), indices(_indices) {}

	void Mesh::GenerateNormals() {
		for (size_t i = 0; i < indices.size(); i += 3) {
			int i0 = indices[i];
			int i1 = indices[i + 1];
			int i2 = indices[i + 2];

			Vertex& v0 = vertices[i0];
            Vertex& v1 = vertices[i1];
            Vertex& v2 = vertices[i2];

			glm::vec3 edge0 = v1.position - v0.position;
			glm::vec3 edge1 = v2.position - v0.position;
			glm::vec3 faceNormal = glm::normalize(glm::cross(edge0, edge1));

			v0.normal += faceNormal;
			v1.normal += faceNormal;
			v2.normal += faceNormal;
		}

		for (Vertex& vertex : vertices)
			vertex.normal = glm::normalize(vertex.normal);
	}
}