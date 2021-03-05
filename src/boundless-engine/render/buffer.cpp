#include "buffer.hpp"
#include "logging/logger.hpp"
#include "platform/opengl/opengl_buffer.hpp"
#include "renderer_api.hpp"

namespace Boundless {

    VertexBuffer* VertexBuffer::create(float* verticies, uint32_t size) {
        switch (RendererAPI::getApi()) {
            case RenderAPI::NONE:
                BD_CORE_ERROR("Renderer API None is not supported");
                throw std::runtime_error("Renderer API None is not supported.");
                break;
            case RenderAPI::OPEN_GL:
                return new OpenGLVertexBuffer(verticies, size);
                break;
        }

        BD_CORE_ERROR("No Renderer API was found");
        throw std::runtime_error("No Renderer API was found");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::create(uint32_t* indicies, uint32_t count) {
        switch (RendererAPI::getApi()) {
            case RenderAPI::NONE:
                BD_CORE_ERROR("Renderer API None is not supported");
                throw std::runtime_error("Renderer API None is not supported.");
                break;
            case RenderAPI::OPEN_GL:
                return new OpenGLIndexBuffer(indicies, count);
                break;
            
            default:
                BD_CORE_ERROR("No Renderer API was found");
                throw std::runtime_error("No Renderer API was found");
                break;
        }
    }


}