#pragma once
#ifndef BD_COMPUTE_BUFFER_HPP_
#define BD_COMPUTE_BUFFER_HPP_

#include <cstdint>
#include "compute_context.hpp"

namespace Boundless {

    enum class ComputeBufferFlags {
        READ_WRITE = 0,
        WRITE_ONLY,
        READ_ONLY,
        HOST_PTR,
        ALLOC_HOST_PTR,
        COPY_HOST_PTR
    };

    class ComputeBuffer {
        public:
            ComputeBuffer();
            ComputeBuffer(const Ref<ComputeContext>& context, size_t flags, size_t bufferSize, void* ptr);
            virtual ~ComputeBuffer() {}

            static ComputeBuffer* create(const Ref<ComputeContext>& context, size_t flags, size_t bufferSize, void* ptr);

    };

}

#endif // !BD_COMPUTE_BUFFER_HPP_
