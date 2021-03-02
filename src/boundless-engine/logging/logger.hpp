#pragma once
#ifndef BD_LOGGER_HPP_
#define BD_LOGGER_HPP_

#include <memory>
#include "spdlog/spdlog.h"

namespace Boundless {

    class Logger {
        public:
            static void init();

            inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return core_logger; }
            inline static std::shared_ptr<spdlog::logger>& getGameLogger() { return game_logger; }
        private:
            static std::shared_ptr<spdlog::logger> core_logger;
            static std::shared_ptr<spdlog::logger> game_logger;
    };

}


#define BD_CORE_ERROR(...) ::Boundless::Logger::getCoreLogger()->error(__VA_ARGS__)
#define BD_CORE_FATAL(...) ::Boundless::Logger::getCoreLogger()->fatal(__VA_ARGS__)
#define BD_CORE_WARN(...)  ::Boundless::Logger::getCoreLogger()->warn(__VA_ARGS__)
#define BD_CORE_INFO(...)  ::Boundless::Logger::getCoreLogger()->info(__VA_ARGS__)
#define BD_CORE_TRACE(...) ::Boundless::Logger::getCoreLogger()->trace(__VA_ARGS__)


#define BD_GAME_ERROR(...) ::Boundless::Logger::getGameLogger()->error(__VA_ARGS__)
#define BD_GAME_FATAL(...) ::Boundless::Logger::getGameLogger()->fatal(__VA_ARGS__)
#define BD_GAME_WARN(...)  ::Boundless::Logger::getGameLogger()->warn(__VA_ARGS__)
#define BD_GAME_INFO(...)  ::Boundless::Logger::getGameLogger()->info(__VA_ARGS__)
#define BD_GAME_TRACE(...) ::Boundless::Logger::getGameLogger()->trace(__VA_ARGS__)


#endif // !BD_LOGGER_HPP_
