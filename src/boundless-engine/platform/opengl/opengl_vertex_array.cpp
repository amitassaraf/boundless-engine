#include "opengl_vertex_array.hpp"
#include "opengl_shader.hpp"
#include "logging/logger.hpp"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

namespace Boundless {

    OpenGLVertexArray::OpenGLVertexArray() {
        glGenVertexArrays(1, &m_rendererId);
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

    void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t beginAtIndex) {
        const auto& layout = vertexBuffer->getLayout();
        if (layout.getElements().size() == 0) {
            BD_CORE_ERROR("Vertex buffer layout was not set.");
            throw std::runtime_error("Vertex buffer layout was not set.");
        }

        this->bind();
        vertexBuffer->bind();
        uint32_t index = beginAtIndex;
        
        for (const auto& element : layout) {
            glVertexAttribPointer(index,
                getShaderDataTypeCount(element.type),
                OpenGLShader::shaderDataTypeToNativeType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                layout.getStride(),
                INT2VOIDP(element.offset));
            glEnableVertexAttribArray(index);
            if (element.instanced) {
                glVertexAttribDivisor(index, 1);
            }
            index++;
        }
        vertexBuffer->unbind();
        this->unbind();
        m_vertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
        this->bind();
        indexBuffer->bind();
        
        m_indexBuffer = indexBuffer;

        indexBuffer->unbind();
        this->unbind();
    }


}