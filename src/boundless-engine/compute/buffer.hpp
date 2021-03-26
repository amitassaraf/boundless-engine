#pragma once
#ifndef BD_COMPUTE_BUFFER_HPP_
#define BD_COMPUTE_BUFFER_HPP_

#include <cstdint>

namespace Boundless {

    class ComputeBuffer {
        public:
            virtual ~ComputeBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            static ComputeBuffer* create();
    };

}

#endif // !BD_COMPUTE_BUFFER_HPP_
