#pragma once
#include <string>
#include "Core.hpp"

namespace mist {
    class MIST_API FileDialog {
    public:
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}