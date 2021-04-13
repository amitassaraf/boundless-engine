#pragma once
#ifndef BD_OPENGL_2DTEXTURE_HPP_
#define BD_OPENGL_2DTEXTURE_HPP_

#include "render/texture.hpp"
#include "logging/logger.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace Boundless {

    class OpenGL2DTexture : public Texture {
        public:
            OpenGL2DTexture(uint16_t width, uint16_t height, TextureColorChannel textureColorChannel, TextureColorChannel renderColorChannel, void* source);
            virtual ~OpenGL2DTexture();

            virtual void bind() const override;
            virtual void unbind() const override;
            virtual void setTextureParameter(TextureParameterName parameter, TextureParameter value) const override;
        protected:
            virtual int textureParameterNameToRendererValue(TextureParameterName format) const override;
            virtual int textureColorChannelToRendererValue(TextureColorChannel channel) const override;
            virtual int textureParameterToRendererValue(TextureParameter format) const override;
            
        private:
            uint32_t m_rendererId;
    };

}

#endif // !BD_OPENGL_2DTEXTURE_HPP_
