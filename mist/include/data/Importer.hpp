#pragma once
#include <string>
#include "Core.hpp"
#include "data/Mesh.hpp"

namespace mist {
    class Importer {
    public:
        static std::vector<Ref<Mesh>> ImportMeshes(const std::string& path);
    };
}