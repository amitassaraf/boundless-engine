#pragma once
#ifndef BD_OPENCL_COMPUTE_PROGRAM_HPP_
#define BD_OPENCL_COMPUTE_PROGRAM_HPP_

#include "compute/compute_program.hpp"

namespace Boundless {

    class OpenCLComputeProgram : public ComputeProgram {
        public:
            OpenCLComputeProgram();
            virtual ~OpenCLComputeProgram();
    };

}

#endif // !BD_OPENCL_COMPUTE_PROGRAM_HPP_
