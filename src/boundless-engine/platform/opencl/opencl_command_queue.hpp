#pragma once
#ifndef BD_OPENCL_COMMAND_QUEUE_HPP_
#define BD_OPENCL_COMMAND_QUEUE_HPP_

#include "compute/compute_command_queue.hpp"
#include "opencl_device.hpp"
#include "opencl_context.hpp"
#include "opencl_program.hpp"
#include "logging/logger.hpp"
#include <vector>

#include <cl.hpp>

namespace Boundless {

    class OpenCLCommandQueue : public ComputeCommandQueue {
        public:
            OpenCLCommandQueue(const Ref<OpenCLContext>& context, const Ref<OpenCLDevice>& device);
            ~OpenCLCommandQueue() override;

            virtual bool enqueueTask(const Ref<ComputeProgram>& program, uint8_t dimensions, size_t offset, size_t globalSize, size_t localSize) const override;
            virtual bool enqueueRead(const Ref<ComputeBuffer>& buffer, bool blocking, size_t offset, size_t bytes, void* ptr) const override;
            virtual void flushCommands() const override;
        private:
            cl_command_queue m_commandQueue;
    };

}

#endif // !BD_OPENCL_COMMAND_QUEUE_HPP_
