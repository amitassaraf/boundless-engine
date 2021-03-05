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

            virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
            virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

            static VertexArray* create();
    };

}


#endif // !BD_VERTEX_ARRAY_HPP_
