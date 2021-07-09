#include "compute_command_queue.hpp"
#include "platform/opencl/opencl_command_queue.hpp"
#include "compute_api.hpp"


namespace Boundless {

    ComputeCommandQueue::ComputeCommandQueue() {}
    ComputeCommandQueue::ComputeCommandQueue(const Ref<ComputeContext>& context, const Ref<ComputeDevice>& device) {
        UNUSED(context);
        UNUSED(device);
    }
    ComputeCommandQueue::~ComputeCommandQueue() {}


    ComputeCommandQueue* ComputeCommandQueue::create(const Ref<ComputeContext>& context, const Ref<ComputeDevice>& device) {
        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLCommandQueue(reinterpret_cast<const Ref<OpenCLContext>&>(context),
                                              reinterpret_cast<const Ref<OpenCLDevice>&>(device));
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}