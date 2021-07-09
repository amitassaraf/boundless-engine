#include "opencl_buffer.hpp"

namespace Boundless {
    OpenCLComputeBuffer::OpenCLComputeBuffer(const Ref<OpenCLContext>& context, size_t flags, size_t bufferSize, void* ptr) {
        cl_int error;

        this->m_buffer = clCreateBuffer(context->getCLContext(),  flags,  bufferSize, ptr, &error);
        if (error != 0) {
            BD_CORE_ERROR("Error creating Compute buffer: {}", error);
        }
    }

    OpenCLComputeBuffer::~OpenCLComputeBuffer() {
        if (this->m_buffer) {
            clReleaseMemObject(m_buffer);
        }
    }
}