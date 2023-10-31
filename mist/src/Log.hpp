#pragma once
#include "Core.hpp"
#include <spdlog/spdlog.h>

namespace mist {
    class Log {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetLogger() { return logger; }
    private:
        static std::shared_ptr<spdlog::logger> logger;
    };
}

// TODO: Add log stripping

// Logging
#define MIST_TRACE(...)			mist::Log::GetLogger()->trace(__VA_ARGS__)
#define MIST_INFO(...)			mist::Log::GetLogger()->info(__VA_ARGS__)
#define MIST_WARN(...)			mist::Log::GetLogger()->warn(__VA_ARGS__)
#define MIST_ERROR(...)			mist::Log::GetLogger()->error(__VA_ARGS__)
#define MIST_CRITICAL(...)		mist::Log::GetLogger()->critical(__VA_ARGS__)