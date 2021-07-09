#pragma once
#ifndef BD_COMPUTE_COMMAND_QUEUE_HPP_
#define BD_COMPUTE_COMMAND_QUEUE_HPP_

#include <string>
#include "core/core.hpp"
#include "compute_device.hpp"
#include "compute_context.hpp"
#include "compute_program.hpp"
#include "buffer.hpp"

namespace Boundless {

    class ComputeCommandQueue {
        public:
            ComputeCommandQueue();
            ComputeCommandQueue(const Ref<ComputeContext>& context, const Ref<ComputeDevice>& device);
            virtual ~ComputeCommandQueue();

            virtual bool enqueueTask(const Ref<ComputeProgram>& program, uint8_t dimensions, size_t offset, size_t globalSize, size_t localSize) const = 0;
            virtual bool enqueueRead(const Ref<ComputeBuffer>& buffer, bool blocking, size_t offset, size_t bytes, void* ptr) const = 0;
            virtual void flushCommands() const = 0;
            static ComputeCommandQueue* create(const Ref<ComputeContext>& context, const Ref<ComputeDevice>& device);
        protected:
            uint32_t m_computeId;
    };

}


#endif // !BD_COMPUTE_COMMAND_QUEUE_HPP_
