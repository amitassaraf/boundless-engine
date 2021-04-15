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
            virtual void clearColor() override;
            virtual void wireframeMode() override;
            virtual void fillMode() override;

            virtual void drawArrays(const Ref<VertexArray>& vertexArray) override;
            virtual void drawIndexed(const Ref<VertexArray>& vertexArray) override;
            virtual void drawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t count) override;
    };

}

#endif // !BD_RENDERER_API_HPP_
