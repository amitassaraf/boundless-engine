#pragma once
#ifndef BD_RENDERER_HPP_
#define BD_RENDERER_HPP_


namespace Boundless {

    enum class RendererAPI {
        NONE = 0,
        OPEN_GL,
    };

    class Renderer {
        public:
            inline static RendererAPI getApi() { return s_renderApi; }
        private:
            static RendererAPI s_renderApi;
    };

}

#endif // !BD_RENDERER_HPP_
