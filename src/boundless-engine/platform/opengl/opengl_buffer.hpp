#pragma once
#ifndef BD_OPENGL_BUFFER_HPP_
#define BD_OPENGL_BUFFER_HPP_

#include "render/buffer.hpp"
#include "logging/logger.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace Boundless {

    class OpenGLVertexBuffer : public VertexBuffer {
        public:
            OpenGLVertexBuffer(float* verticies, uint32_t size);
            virtual ~OpenGLVertexBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;

            virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
            virtual const BufferLayout& getLayout() const override { return m_layout; }

        private:
            uint32_t m_rendererId;
            BufferLayout m_layout;
    };

    class OpenGLIndexBuffer : public IndexBuffer {
        public:
            OpenGLIndexBuffer(uint32_t* indicies, uint32_t count);
            virtual ~OpenGLIndexBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
            virtual uint32_t getCount() const override { return m_count; }
        private:
            uint32_t m_rendererId;
            uint32_t m_count;
    };

}

#endif // !BD_OPENGL_BUFFER_HPP_
