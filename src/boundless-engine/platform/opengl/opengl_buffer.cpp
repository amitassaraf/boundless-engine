#include "opengl_buffer.hpp"

namespace Boundless {

    //////////////////////////////////////////////////////////////////////////////////////////////
    // OpenGLVertexBuffer
    //////////////////////////////////////////////////////////////////////////////////////////////

    OpenGLVertexBuffer::OpenGLVertexBuffer(float* verticies, uint32_t size) {
        glGenBuffers(1, &m_rendererId);
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);  
        glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        glDeleteBuffers(1, &m_rendererId);
    }


    void OpenGLVertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);  
    }

    void OpenGLVertexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    // OpenGLIndexBuffer
    //////////////////////////////////////////////////////////////////////////////////////////////

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indicies, uint32_t count) : m_count(count) {
        glGenBuffers(1, &m_rendererId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);  
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indicies, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        glDeleteBuffers(1, &m_rendererId);
    }

    void OpenGLIndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);  
    }

    void OpenGLIndexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
    }
}