#pragma once
#ifndef BND_LOGGER_HPP_
#define BND_LOGGER_HPP_

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


#endif // !BND_LOGGER_HPP_
