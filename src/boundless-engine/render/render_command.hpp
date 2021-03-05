#pragma once
#ifndef BD_RENDER_COMMAND_HPP_
#define BD_RENDER_COMMAND_HPP_

#include <memory>
#include "renderer_api.hpp"

namespace Boundless {


    class RenderCommand {
        public:
            inline static void setClearColor(const glm::vec4& color) {
                s_rendererApi->setClearColor(color);
            }

            inline static void clear() {
                s_rendererApi->clear();
            }

            inline static void drawIndexed(const Ref<VertexArray>& vertexArray) {
                s_rendererApi->drawIndexed(vertexArray);
            }

            inline static void wireframeMode() {
                s_rendererApi->wireframeMode();
            }

            inline static void fillMode() {
                s_rendererApi->fillMode();
            }
        private:
            static RendererAPI* s_rendererApi;
    };

}

#endif // !BD_RENDER_COMMAND_HPP_
