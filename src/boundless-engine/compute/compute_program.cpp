#include "compute_program.hpp"
#include "platform/opencl/opencl_compute_program.hpp"
#include "compute_api.hpp"
#include <fstream>

namespace Boundless {

    ComputeProgram::ComputeProgram() {

    }

    ComputeProgram::~ComputeProgram() {

    }


    ComputeProgram* ComputeProgram::create() {
        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLComputeProgram();
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}