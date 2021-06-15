#include "opengl_buffer.hpp"

namespace Boundless {

    //////////////////////////////////////////////////////////////////////////////////////////////
    // OpenGLVertexBuffer
    //////////////////////////////////////////////////////////////////////////////////////////////

    OpenGLVertexBuffer::OpenGLVertexBuffer(float* verticies, uint32_t size) : m_size(size) {
        glGenBuffers(1, &m_rendererId);
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);  
        glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint64_t* verticies, uint32_t size) : m_size(size) {
        glGenBuffers(1, &m_rendererId);
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
        glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  
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

    //////////////////////////////////////////////////////////////////////////////////////////////
    // OpenGLFrameBuffer
    //////////////////////////////////////////////////////////////////////////////////////////////

    OpenGLFrameBuffer::OpenGLFrameBuffer() {
        glGenFramebuffers(1, &m_rendererId);
        this->bind();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer() {
        glDeleteFramebuffers(1, &m_rendererId);
    }

    void OpenGLFrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);  
    }

    void OpenGLFrameBuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    }

    void OpenGLFrameBuffer::set2DTexture(unsigned int index, Ref<Texture> texture) const {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->getRendererID(), 0);  
    }

    void OpenGLFrameBuffer::setRenderBuffer(FrameBufferAttachmentType type, Ref<RenderBuffer> renderBuffer) const {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, this->frameBufferAttachmentTypeToRendererValue(type), GL_RENDERBUFFER, renderBuffer->getRendererID());  
    }

    void OpenGLFrameBuffer::enableTextureIndexes(unsigned int indexes[], unsigned int amount) const {
        unsigned int glIndexes[amount];
        for (unsigned int i = 0; i < amount; i++) {
            glIndexes[i] = GL_COLOR_ATTACHMENT0 + indexes[i];
        }
        glDrawBuffers(amount, glIndexes);
    }

    unsigned int OpenGLFrameBuffer::frameBufferAttachmentTypeToRendererValue(FrameBufferAttachmentType type) const {
        switch(type) {
            case FrameBufferAttachmentType::DEPTH:
                return GL_DEPTH_ATTACHMENT;
        }

        BD_CORE_ERROR("Unsupported type");
        throw std::runtime_error("Unsupported type");
    }


    //////////////////////////////////////////////////////////////////////////////////////////////
    // OpenGLRenderBuffer
    //////////////////////////////////////////////////////////////////////////////////////////////

    OpenGLRenderBuffer::OpenGLRenderBuffer(RenderBufferType type, int width, int height) {
        glGenRenderbuffers(1, &m_rendererId);
        this->bind();
        glRenderbufferStorage(GL_RENDERBUFFER, this->renderBufferTypeToRendererValue(type), width, height);
    }

    OpenGLRenderBuffer::~OpenGLRenderBuffer() {
        glDeleteRenderbuffers(1, &m_rendererId);
    }

    void OpenGLRenderBuffer::bind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, m_rendererId);  
    }

    void OpenGLRenderBuffer::unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);  
    }

    unsigned int OpenGLRenderBuffer::renderBufferTypeToRendererValue(RenderBufferType type) const {
        switch(type) {
            case RenderBufferType::DEPTH_BUFFER:
                return GL_DEPTH_COMPONENT;
        }

        BD_CORE_ERROR("Unsupported type");
        throw std::runtime_error("Unsupported type");
    }

}