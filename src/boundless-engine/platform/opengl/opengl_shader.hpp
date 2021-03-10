#pragma once
#ifndef BD_OPENGL_SHADER_HPP_
#define BD_OPENGL_SHADER_HPP_

#include "render/shader.hpp"
#include "logging/logger.hpp"
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Boundless {

    class OpenGLShader : public Shader {
        public:
            OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
            virtual ~OpenGLShader();

            virtual void bind() const override;
            virtual void unbind() const override;
            virtual LocatedUniform* locateUniform(const std::string& uniformName) const override;
            virtual void setUniform(const Ref<LocatedUniform>& uniformLocation, const glm::mat4& uniform) const override;
            virtual void setUniform(const std::string& uniformName, const glm::mat4& uniform) const override;
            virtual void setUniform(const std::string& uniformName, const glm::vec4& uniform) const override;
            virtual void setUniform(const std::string& uniformName, const glm::vec3& uniform) const override;
            static unsigned int shaderDataTypeToNativeType(ShaderDataType type);
    };

}

#endif // !BD_OPENGL_SHADER_HPP_
