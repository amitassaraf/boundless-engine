#include "opengl_buffer.hpp"

namespace Boundless {

    OpenGLVertexBuffer::OpenGLVertexBuffer(float* verticies, uint32_t size) {
        glCreateBuffers(1, &m_rendererId);
        glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indicies, uint32_t size) {
        glCreateBuffers(1, &m_rendererId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicies, GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        glDeleteBuffers(1, &m_rendererId);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        glDeleteBuffers(1, &m_rendererId);
    }

    void OpenGLVertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);  
    }

    void OpenGLVertexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
    }


    void OpenGLIndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);  
    }

    void OpenGLIndexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
    }
}