#pragma once
#ifndef BD_OPENCL_SHADER_HPP_
#define BD_OPENCL_SHADER_HPP_

#include "compute/compute_program.hpp"
#include "opencl_device.hpp"
#include "opencl_context.hpp"
#include "opencl_buffer.hpp"
#include "logging/logger.hpp"
#include <vector>

#include <cl.hpp>

namespace Boundless {

    class OpenCLProgram : public ComputeProgram {
    public:
        OpenCLProgram(const Ref<OpenCLContext> &context, const Ref<OpenCLDevice> &device, const std::string &shaderSrc,
                      const std::string &entrypoint);

        ~OpenCLProgram() override;

        bool addArgument(uint32_t index, const Ref<ComputeBuffer>& buffer) const override;
        bool addArgument(uint32_t index, size_t bytes, void* ptr) const override;

        [[nodiscard]] cl_kernel getCLKernel() const { return m_kernel; };
    private:
        cl_program m_program;
        cl_kernel m_kernel;
    };

}

#endif // !BD_OPENCL_SHADER_HPP_
