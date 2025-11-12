#pragma once
#include "Log.hpp"
#include "Instrumentor.hpp"

#if DEBUG
#if defined(_MSC_VER)
	#define MIST_ASSERT(x, ...) { if(!(x)) { MIST_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#elif defined(__GNUC__) || defined(__clang__)
	#define MIST_ASSERT(x, ...) { if(!(x)) { MIST_ERROR("Assertion Failed: {0}", __VA_ARGS__); __builtin_trap(); } }
#endif
#else
	#define MIST_ASSERT(x, ...)
#endif