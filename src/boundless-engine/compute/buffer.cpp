#include "buffer.hpp"
#include "logging/logger.hpp"
#include "platform/opencl/opencl_buffer.hpp"
#include "compute_api.hpp"

namespace Boundless {

    ComputeBuffer::ComputeBuffer() = default;
    ComputeBuffer::ComputeBuffer(const Ref<ComputeContext>& context, size_t flags, size_t bufferSize, void* ptr) {
        UNUSED(context);
        UNUSED(bufferSize);
        UNUSED(flags);
        UNUSED(ptr);
    }

    ComputeBuffer* ComputeBuffer::create(const Ref<ComputeContext>& context, size_t flags, size_t bufferSize, void* ptr) {
        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLComputeBuffer(reinterpret_cast<const Ref<OpenCLContext>&>(context), flags, bufferSize, ptr);
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}