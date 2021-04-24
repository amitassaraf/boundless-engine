#pragma once
#ifndef BD_SHADER_HPP_
#define BD_SHADER_HPP_

#include <string>
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <glm/glm.hpp>

namespace Boundless {

    enum class ShaderDataType {
        NONE = 0, VEC1, VEC2, VEC3, VEC4, MAT3, MAT4, IVEC1, IVEC2, IVEC3, IVEC4, BOOL
    };

    uint32_t getShaderDataTypeSize(ShaderDataType type);
    uint32_t getShaderDataTypeCount(ShaderDataType type);

    struct LocatedUniform {
        std::string name;
        unsigned int location;
        LocatedUniform() {}
        LocatedUniform(const std::string& name, unsigned int location) : name(name), location(location) {}
    };
    
    class Shader {
        public:
            Shader();
            Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
            virtual ~Shader();

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            virtual LocatedUniform* locateUniform(const std::string& uniformName) const = 0;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const glm::mat4& uniform) const = 0;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const glm::vec4& uniform) const = 0;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const glm::vec3& uniform) const = 0;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const glm::vec2& uniform) const = 0;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const float uniform) const = 0;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const int uniform) const = 0;
            virtual void setUniform(const std::string& uniformName, const glm::mat4& uniform) const = 0;
            virtual void setUniform(const std::string& uniformName, const glm::vec4& uniform) const = 0;
            virtual void setUniform(const std::string& uniformName, const glm::vec3& uniform) const = 0;
            virtual void setUniform(const std::string& uniformName, const glm::vec2& uniform) const = 0;
            virtual void setUniform(const std::string& uniformName, const float uniform) const = 0;
            virtual void setUniform(const std::string& uniformName, const int uniform) const = 0;
            virtual void setActiveTextureUnit(unsigned int index) const = 0;
            static unsigned int shaderDataTypeToNativeType(ShaderDataType type);

            static Shader* create(const std::string& filepath);
        protected:
            uint32_t m_rendererId;
    };

}


#endif // !BD_SHADER_HPP_
