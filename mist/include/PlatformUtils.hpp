#pragma once
#include <string>
#include "Core.hpp"

namespace mist {
    class FileDialog {
    public:
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}