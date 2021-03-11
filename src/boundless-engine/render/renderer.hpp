#pragma once
#ifndef BD_RENDERER_HPP_
#define BD_RENDERER_HPP_

#include <memory>
#include "vertex_array.hpp"

namespace Boundless {


    class Renderer {
        public:
            static void beginScene();
            static void endScene();
            static void submit(const Ref<VertexArray>& vertexArray);
            static void submitInstanced(const Ref<VertexArray>& vertexArray, uint32_t count);
    };

}

#endif // !BD_RENDERER_HPP_
