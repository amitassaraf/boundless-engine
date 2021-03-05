#pragma once
#ifndef BD_OPENGL_RENDERER_API_HPP_
#define BD_OPENGL_RENDERER_API_HPP_

#include "render/renderer_api.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Boundless {

    class OpenGLRendererAPI : public RendererAPI {
        public:
            virtual void setClearColor(const glm::vec4& color) override;
            virtual void clear() override;
            virtual void wireframeMode() override;
            virtual void fillMode() override;

            virtual void drawIndexed(const Ref<VertexArray>& vertexArray) override;
    };

}

#endif // !BD_RENDERER_API_HPP_
