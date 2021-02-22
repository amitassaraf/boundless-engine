#include "logging/logger.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"


namespace Boundless {
    std::shared_ptr<spdlog::logger> Logger::core_logger;
    std::shared_ptr<spdlog::logger> Logger::game_logger;

    void Logger::init() {
        spdlog::set_pattern("[%H:%M:%S %z] %^ [%n] [%L] %v %$");
        
        core_logger = spdlog::stdout_color_mt("Boundless");
        core_logger->set_level(spdlog::level::trace);
        
        game_logger = spdlog::stdout_color_mt("Game");
        game_logger->set_level(spdlog::level::trace);
    }
}