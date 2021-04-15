#pragma once
#ifndef BD_RENDERER_API_HPP_
#define BD_RENDERER_API_HPP_

#include <glm/vec4.hpp>
#include <memory>
#include "vertex_array.hpp"

namespace Boundless {


    enum class RenderAPI {
        NONE = 0,
        OPEN_GL,
    };

    class RendererAPI {
        public:
            virtual void setClearColor(const glm::vec4& color) = 0;
            virtual void clear() = 0;
            virtual void clearColor() = 0;
            virtual void wireframeMode() = 0;
            virtual void fillMode() = 0;
            virtual void drawArrays(const Ref<VertexArray>& vertexArray) = 0;
            virtual void drawIndexed(const Ref<VertexArray>& vertexArray) = 0;
            virtual void drawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t count) = 0;

            inline static RenderAPI getApi() { return s_Api; }
        private:
            static RenderAPI s_Api;
    };

}

#endif // !BD_RENDERER_API_HPP_
