#include "renderer.hpp"
#include "render_command.hpp"

namespace Boundless {

    void Renderer::beginScene() {

    }
    
    void Renderer::endScene() {

    }

    void Renderer::submit(const Ref<VertexArray>& vertexArray) {
        RenderCommand::drawIndexed(vertexArray);
    }

    void Renderer::submitInstanced(const Ref<VertexArray>& vertexArray, uint32_t count) {
        RenderCommand::drawIndexedInstanced(vertexArray, count);
    }
}