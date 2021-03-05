#pragma once
#ifndef BD_VERTEX_ARRAY_HPP_
#define BD_VERTEX_ARRAY_HPP_

#include <memory>
#include "core/core.hpp"
#include "logging/logger.hpp"
#include "buffer.hpp"

namespace Boundless {

    class VertexArray {
        public:
            virtual ~VertexArray() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
            virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

            virtual const Ref<IndexBuffer>& getIndexBuffer() const = 0; 

            static VertexArray* create();
    };

}


#endif // !BD_VERTEX_ARRAY_HPP_
