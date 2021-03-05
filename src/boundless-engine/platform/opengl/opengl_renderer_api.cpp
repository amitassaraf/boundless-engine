#include "opengl_renderer_api.hpp"
#include "logging/logger.hpp"

namespace Boundless {


    void OpenGLRendererAPI::setClearColor(const glm::vec4& color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }
    
    void OpenGLRendererAPI::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertexArray) {
        glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::wireframeMode() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void OpenGLRendererAPI::fillMode() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

}