#pragma once
#ifndef BD_BUFFER_HPP_
#define BD_BUFFER_HPP_

#include <cstdint>

namespace Boundless {

    class VertexBuffer {
        public:
            virtual ~VertexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            static VertexBuffer* create(float* verticies, uint32_t size);
    };

    class IndexBuffer {
        public:
            virtual ~IndexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            static IndexBuffer* create(uint32_t* indicies, uint32_t size);
    };

}

#endif // !BD_BUFFEr_HPP_
