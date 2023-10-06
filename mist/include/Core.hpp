#pragma once
#include <memory>

#ifdef MIST_DLL
    #define MIST_API __declspec(dllexport)
#else
    #define MIST_API __declspec(dllimport)
#endif

#if _WIN32
#elif __linux__
    #error "Linux not supported yet"
#else
    #error "Unsupported Platform"
#endif

namespace mist {
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
