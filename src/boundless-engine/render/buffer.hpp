#pragma once
#ifndef BD_BUFFER_HPP_
#define BD_BUFFER_HPP_

#include <cstdint>
#include "buffer_layout.hpp"

namespace Boundless {

    class VertexBuffer {
        public:
            virtual ~VertexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            virtual void setLayout(const BufferLayout& layout) = 0;
            virtual const BufferLayout& getLayout() const = 0;

            static VertexBuffer* create(float* verticies, uint32_t size);
    };

    class IndexBuffer {
        public:
            virtual ~IndexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            virtual uint32_t getCount() const = 0;

            static IndexBuffer* create(uint32_t* indicies, uint32_t count);
    };

}

#endif // !BD_BUFFER_HPP_
