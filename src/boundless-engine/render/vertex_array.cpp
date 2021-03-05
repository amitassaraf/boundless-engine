#include "vertex_array.hpp"
#include "platform/opengl/opengl_vertex_array.hpp"
#include "renderer_api.hpp"

namespace Boundless {

    VertexArray* VertexArray::create() {
        switch (RendererAPI::getApi()) {
            case RenderAPI::NONE:
                BD_CORE_ERROR("Renderer API None is not supported");
                throw std::runtime_error("Renderer API None is not supported.");
                break;
            case RenderAPI::OPEN_GL:
                return new OpenGLVertexArray();
                break;
        }

        BD_CORE_ERROR("No Renderer API was found");
        throw std::runtime_error("No Renderer API was found");
        return nullptr;
    }


}