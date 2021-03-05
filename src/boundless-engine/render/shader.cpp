#include "shader.hpp"

namespace Boundless {

    unsigned int Shader::shaderDataTypeToNativeType(ShaderDataType type) {
        UNUSED(type);

        BD_CORE_ERROR("Unimplemented error.");
        throw std::runtime_error("Unimplemented error.");
        return 0;
    }

    uint32_t getShaderDataTypeSize(ShaderDataType type) {
        switch (type)
        {
            case ShaderDataType::VEC1:      return sizeof(float);
            case ShaderDataType::VEC2:      return sizeof(float) * 2;
            case ShaderDataType::VEC3:      return sizeof(float) * 3;
            case ShaderDataType::VEC4:      return sizeof(float) * 4;
            case ShaderDataType::MAT3:      return sizeof(float) * 3 * 3;
            case ShaderDataType::MAT4:      return sizeof(float) * 4 * 4;
            case ShaderDataType::IVEC1:     return sizeof(int);
            case ShaderDataType::IVEC2:     return sizeof(int) * 2;
            case ShaderDataType::IVEC3:     return sizeof(int) * 3;
            case ShaderDataType::IVEC4:     return sizeof(int) * 4;
            case ShaderDataType::BOOL:      return 1;
            case ShaderDataType::NONE:      break;
        }

        BD_CORE_ERROR("Unknown Shader Data Type.");
        throw std::runtime_error("Unknown Shader Data Type.");
        return 0;
    }

    uint32_t getShaderDataTypeCount(ShaderDataType type) {
        switch (type)
        {
            case ShaderDataType::VEC1:      return 1;
            case ShaderDataType::VEC2:      return 2;
            case ShaderDataType::VEC3:      return 3;
            case ShaderDataType::VEC4:      return 4;
            case ShaderDataType::MAT3:      return 3 * 3;
            case ShaderDataType::MAT4:      return 4 * 4;
            case ShaderDataType::IVEC1:     return 1;
            case ShaderDataType::IVEC2:     return 2;
            case ShaderDataType::IVEC3:     return 3;
            case ShaderDataType::IVEC4:     return 4;
            case ShaderDataType::BOOL:      return 1;
            case ShaderDataType::NONE:      break;
        }

        BD_CORE_ERROR("Unknown Shader Data Type.");
        throw std::runtime_error("Unknown Shader Data Type.");
        return 0;
    }

    Shader::Shader() {}
    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        UNUSED(vertexSrc);
        UNUSED(fragmentSrc);
    }
    Shader::~Shader() {}

}