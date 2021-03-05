#pragma once
#ifndef BD_SHADER_HPP_
#define BD_SHADER_HPP_

#include <string>
#include "core/core.hpp"
#include "logging/logger.hpp"

namespace Boundless {

    enum class ShaderDataType {
        NONE = 0, VEC1, VEC2, VEC3, VEC4, MAT3, MAT4, IVEC1, IVEC2, IVEC3, IVEC4, BOOL
    };

    uint32_t getShaderDataTypeSize(ShaderDataType type);
    uint32_t getShaderDataTypeCount(ShaderDataType type);
    
    class Shader {
        public:
            Shader();
            Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
            virtual ~Shader();

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            static unsigned int shaderDataTypeToNativeType(ShaderDataType type);

            static Shader* create(const std::string& filepath);
        protected:
            uint32_t m_rendererId;
    };

}


#endif // !BD_SHADER_HPP_
