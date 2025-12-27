#pragma once
#include <string>
#include "Core.hpp"

namespace mist {
    class FileDialog {
    public:
        static std::string OpenFile(const std::string& filter);
        static std::string SaveFile(const std::string& filter);
    };
}