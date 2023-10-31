#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mist {
    std::shared_ptr<spdlog::logger> Log::logger;

    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
		logger = spdlog::stdout_color_mt("mist");
		logger->set_level(spdlog::level::trace);
    }
}