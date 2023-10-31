#pragma once
#include "Log.hpp"
#include "Instrumentor.hpp"

#if DEBUG
	#define MIST_ASSERT(x, ...) { if(!(x)) { MIST_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define MIST_ASSERT(x, ...)
#endif