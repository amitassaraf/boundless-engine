#pragma once
#ifndef BD_OPENCL_BUFFER_HPP_
#define BD_OPENCL_BUFFER_HPP_

#include "compute/buffer.hpp"
#include "logging/logger.hpp"
#include "opencl_context.hpp"
#include <OpenCL/opencl.h>

namespace Boundless {

    class OpenCLComputeBuffer : public ComputeBuffer {
        public:
            OpenCLComputeBuffer(const Ref<OpenCLContext>& context, size_t flags, size_t bufferSize, void* ptr);
            ~OpenCLComputeBuffer() override;

            [[nodiscard]] cl_mem getCLBuffer() const { return m_buffer; };
        private:
            cl_mem m_buffer;
    };
}

#endif // !BD_OPENCL_BUFFER_HPP_
