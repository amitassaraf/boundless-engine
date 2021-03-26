#pragma once
#ifndef BD_COMPUTE_PROGRAM_HPP_
#define BD_COMPUTE_PROGRAM_HPP_

#include <memory>
#include "logging/logger.hpp"

namespace Boundless {

    class ComputeProgram {
        public:
            ComputeProgram();
            virtual ~ComputeProgram();

            static ComputeProgram* create();
    };

}

#endif // !BD_COMPUTE_PROGRAM_HPP_
