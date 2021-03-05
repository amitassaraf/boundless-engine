#pragma once
#ifndef BD_OPENGL_VERTEX_ARRAY_HPP_
#define BD_OPENGL_VERTEX_ARRAY_HPP_

#include "render/vertex_array.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace Boundless {

    class OpenGLVertexArray : public VertexArray {
        public:
            OpenGLVertexArray();
            virtual ~OpenGLVertexArray();

            virtual void bind() const override;
            virtual void unbind() const override;

            virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
            virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
            
        private:
            std::vector< std::shared_ptr<VertexBuffer> > m_vertexBuffers;
            std::shared_ptr< IndexBuffer > m_indexBuffer;

            uint32_t m_rendererId;
    };

}

#endif // !BD_OPENGL_VERTEX_ARRAY_HPP_
