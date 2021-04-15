#pragma once
#ifndef BD_TEXTURE_HPP_
#define BD_TEXTURE_HPP_

#include <cstdint>

namespace Boundless {

    enum class TextureParameterName {
        WRAP_S = 0, WRAP_T, MIN_FILTER, MAG_FILTER
    };

    enum class TextureColorChannel {
        RGB = 0, RGBA, RGBA16F, RED, RGBA32F
    };

    enum class TextureParameter {
        NEAREST = 0, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR, CLAMP_TO_EDGE, REPEAT,
    };

    enum class TextureDataType {
        FLOAT = 0, UNSIGNED_BYTE,
    };

    class Texture {
        public:
            virtual ~Texture() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            virtual void setTextureParameter(TextureParameterName parameter, TextureParameter value) const = 0;
            inline uint32_t getRendererID() const { return m_rendererId;  }

            static Texture* create2DTexture(uint16_t width, uint16_t height, TextureColorChannel textureColorChannel, TextureColorChannel renderColorChannel, TextureDataType type, void* source);
        protected:
            virtual unsigned int textureParameterNameToRendererValue(TextureParameterName format) const  = 0;
            virtual unsigned int textureColorChannelToRendererValue(TextureColorChannel channel) const  = 0;
            virtual unsigned int textureParameterToRendererValue(TextureParameter format) const  = 0;
            virtual unsigned int textureDataTypeToRendererValue(TextureDataType type) const = 0;
            uint32_t m_rendererId;
    };

}

#endif // !BD_TEXTURE_HPP_
