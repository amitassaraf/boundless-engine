#include "texture.hpp"
#include "logging/logger.hpp"
#include "platform/opengl/opengl_2d_texture.hpp"
#include "renderer_api.hpp"

namespace Boundless {

    Texture* Texture::create2DTexture(uint16_t width, uint16_t height, TextureColorChannel textureColorChannel, TextureColorChannel renderColorChannel, void* source) {
        switch (RendererAPI::getApi()) {
            case RenderAPI::NONE:
                BD_CORE_ERROR("Renderer API None is not supported");
                throw std::runtime_error("Renderer API None is not supported.");
                break;
            case RenderAPI::OPEN_GL:
                return new OpenGL2DTexture(width, height, textureColorChannel, renderColorChannel, source);
                break;
        }

        BD_CORE_ERROR("No Renderer API was found");
        throw std::runtime_error("No Renderer API was found");
        return nullptr;
    }

}