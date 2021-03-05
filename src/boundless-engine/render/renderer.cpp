#include "renderer.hpp"
#include "render_command.hpp"

namespace Boundless {

    void Renderer::beginScene() {

    }
    
    void Renderer::endScene() {

    }

    void submit(const Ref<VertexArray>& vertexArray) {
        vertexArray->bind();
        RenderCommand::drawIndexed(vertexArray);
    }
}