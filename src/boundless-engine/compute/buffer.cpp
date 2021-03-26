#include "buffer.hpp"
#include "logging/logger.hpp"
#include "platform/opencl/opencl_buffer.hpp"
#include "compute_api.hpp"

namespace Boundless {

    ComputeBuffer* ComputeBuffer::create() {
        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLComputeBuffer();
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}