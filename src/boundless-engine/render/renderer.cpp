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
}