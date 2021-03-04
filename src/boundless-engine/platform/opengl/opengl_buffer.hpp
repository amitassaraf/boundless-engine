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
        private:
            uint32_t m_rendererId;
    };

    class OpenGLIndexBuffer : public IndexBuffer {
        public:
            OpenGLIndexBuffer(uint32_t* indicies, uint32_t size);
            virtual ~OpenGLIndexBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
        private:
            uint32_t m_rendererId;
    };

}

#endif // !BD_OPENGL_BUFFER_HPP_
