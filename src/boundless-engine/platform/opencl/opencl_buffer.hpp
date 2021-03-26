#pragma once
#ifndef BD_OPENCL_BUFFER_HPP_
#define BD_OPENCL_BUFFER_HPP_

#include "compute/buffer.hpp"
#include "logging/logger.hpp"

namespace Boundless {

    class OpenCLComputeBuffer : public ComputeBuffer {
        public:
            OpenCLComputeBuffer();
            virtual ~OpenCLComputeBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
    };
}

#endif // !BD_OPENCL_BUFFER_HPP_
