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

            virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t beginAtIndex = 0) override;
            virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
            
            inline virtual const Ref<IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; }
        private:
            std::vector< Ref<VertexBuffer> > m_vertexBuffers;
            Ref< IndexBuffer > m_indexBuffer;

            uint32_t m_rendererId;
    };

}

#endif // !BD_OPENGL_VERTEX_ARRAY_HPP_
