#pragma once
#ifndef BD_OPENCL_CONTEXT_HPP_
#define BD_OPENCL_CONTEXT_HPP_

#include "compute/compute_context.hpp"
#include "opencl_device.hpp"
#include "logging/logger.hpp"
#include <vector>

#include <cl.hpp>

namespace Boundless {

    class OpenCLContext : public ComputeContext {
        public:
            explicit OpenCLContext(const Ref<OpenCLDevice>& device);
            virtual ~OpenCLContext();

            [[nodiscard]] cl_context getCLContext() const { return m_context; }
        private:
            cl_context m_context;
    };

}

#endif // !BD_OPENCL_CONTEXT_HPP_
