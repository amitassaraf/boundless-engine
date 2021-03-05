#include "opengl_vertex_array.hpp"
#include "opengl_shader.hpp"
#include "logging/logger.hpp"

namespace Boundless {

    OpenGLVertexArray::OpenGLVertexArray() {
        glCreateVertexArrays(1, &m_rendererId);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_rendererId);
    }


    void OpenGLVertexArray::bind() const {
        glBindVertexArray(m_rendererId);
    }

    void OpenGLVertexArray::unbind() const {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
        const auto& layout = vertexBuffer->getLayout();
        if (layout.getElements().size() == 0) {
            BD_CORE_ERROR("Vertex buffer layout was not set.");
            throw std::runtime_error("Vertex buffer layout was not set.");
        }

        this->bind();
        vertexBuffer->bind();
        uint32_t index = 0;
        
        for (const auto& element : layout) {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                getShaderDataTypeCount(element.type),
                OpenGLShader::shaderDataTypeToNativeType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                layout.getStride(),
                (const void *)&element.offset);
            index++;
        }

        m_vertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
        this->bind();
        indexBuffer->bind();
        
        m_indexBuffer = indexBuffer;
    }


}