#include "compute_program.hpp"
#include "platform/opencl/opencl_context.hpp"
#include "compute_api.hpp"
#include <fstream>


namespace Boundless {

    ComputeContext::ComputeContext() {}
    ComputeContext::ComputeContext(const Ref<ComputeDevice>& device) {
        UNUSED(device);
    }
    ComputeContext::~ComputeContext() {}


    ComputeContext* ComputeContext::create(const Ref<ComputeDevice>& device) {
        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLContext(reinterpret_cast<const Ref<OpenCLDevice>&>(device));
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}