#include "opengl_2d_texture.hpp"
#include "core/core.hpp"

namespace Boundless {

    OpenGL2DTexture::OpenGL2DTexture(uint16_t width, uint16_t height, TextureColorChannel textureColorChannel, TextureColorChannel renderColorChannel, TextureDataType type, void* source) {
        glGenTextures(1, &m_rendererId);
        this->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, 
            this->textureColorChannelToRendererValue(renderColorChannel), width, height, 0, 
            this->textureColorChannelToRendererValue(textureColorChannel), 
            this->textureDataTypeToRendererValue(type), source);
        this->unbind();
    }

    OpenGL2DTexture::~OpenGL2DTexture() {
        glDeleteTextures(1, &m_rendererId);
    }

    void OpenGL2DTexture::setTextureParameter(TextureParameterName parameter, TextureParameter value) const {
        glTexParameteri(GL_TEXTURE_2D, this->textureParameterNameToRendererValue(parameter), this->textureParameterToRendererValue(value));
    }

    void OpenGL2DTexture::bind() const {
        glBindTexture(GL_TEXTURE_2D, m_rendererId);
    }

    void OpenGL2DTexture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);  
    }

    unsigned int OpenGL2DTexture::textureColorChannelToRendererValue(TextureColorChannel format) const {
        switch(format) {
            case TextureColorChannel::RGB:
                return GL_RGB;
            case TextureColorChannel::RGBA:
                return GL_RGBA;
            case TextureColorChannel::RED:
                return GL_RED;
            case TextureColorChannel::RGBA16F:
                return GL_RGBA16F;
            case TextureColorChannel::RGBA32F:
                return GL_RGBA32F;
        }

        BD_CORE_ERROR("Unsupported format");
        throw std::runtime_error("Unsupported format");
    }
    
    unsigned int OpenGL2DTexture::textureParameterNameToRendererValue(TextureParameterName parameter) const {
        switch(parameter) {
            case TextureParameterName::WRAP_S:
                return GL_TEXTURE_WRAP_S;
            case TextureParameterName::WRAP_T:
                return GL_TEXTURE_WRAP_T;
            case TextureParameterName::WRAP_R:
                return GL_TEXTURE_WRAP_R;
            case TextureParameterName::MIN_FILTER:
                return GL_TEXTURE_MIN_FILTER;
            case TextureParameterName::MAG_FILTER:
                return GL_TEXTURE_MAG_FILTER;
            case TextureParameterName::COMPARE_MODE:
                return GL_TEXTURE_COMPARE_MODE;
        }

        BD_CORE_ERROR("Unsupported parameter");
        throw std::runtime_error("Unsupported parameter");
    }

    unsigned int OpenGL2DTexture::textureParameterToRendererValue(TextureParameter function) const {
        switch(function) {
            case TextureParameter::NEAREST:
                return GL_NEAREST;
            case TextureParameter::LINEAR:
                return GL_LINEAR;
            case TextureParameter::LINEAR_MIPMAP_LINEAR:
                return GL_LINEAR_MIPMAP_LINEAR;
            case TextureParameter::LINEAR_MIPMAP_NEAREST:
                return GL_LINEAR_MIPMAP_NEAREST;
            case TextureParameter::NEAREST_MIPMAP_LINEAR:
                return GL_NEAREST_MIPMAP_LINEAR;
            case TextureParameter::NEAREST_MIPMAP_NEAREST:
                return GL_NEAREST_MIPMAP_NEAREST;
            case TextureParameter::REPEAT:
                return GL_REPEAT;
            case TextureParameter::CLAMP_TO_EDGE:
                return GL_CLAMP_TO_EDGE;
            case TextureParameter::NONE:
                return GL_NONE;
        }

        BD_CORE_ERROR("Unsupported function");
        throw std::runtime_error("Unsupported function");
    }

    unsigned int OpenGL2DTexture::textureDataTypeToRendererValue(TextureDataType type) const {
        switch(type) {
            case TextureDataType::FLOAT:
                return GL_FLOAT;
            case TextureDataType::UNSIGNED_BYTE:
                return GL_UNSIGNED_BYTE;
        }

        BD_CORE_ERROR("Unsupported type");
        throw std::runtime_error("Unsupported type");
    }

}