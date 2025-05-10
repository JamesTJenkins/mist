#pragma once
#include <memory>

#if _WIN32
	#ifdef MIST_DLL
		#define MIST_API __declspec(dllexport)
	#else
		#define MIST_API __declspec(dllimport)
	#endif
#elif __linux__
	#define MIST_API __attribute__((visibility("default")))
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
