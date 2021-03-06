#include "opengl_shader.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Boundless {

    void verifyShaderCompiled(GLuint shaderId) {
        GLint  isCompiled;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == GL_FALSE) {
            int maxLength = 0;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(shaderId);

            BD_CORE_ERROR("Vertex Shader Compilation Error: {}", infoLog.data());
            throw std::runtime_error("Shared compilation failed.");
        }
    }


    void verifyProgramLinked(GLuint program) {
        GLint  isLinked;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

        if (isLinked == GL_FALSE) {
            int maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(program);
            
            BD_CORE_ERROR("Program linkage error: {}", infoLog.data());
            throw std::runtime_error("Program linkage failed.");
        }

    }

    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

        const GLchar* vertexSource = vertexSrc.c_str();
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        
        verifyShaderCompiled(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        const GLchar* fragmentSource = fragmentSrc.c_str();
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        m_rendererId = glCreateProgram();
        glAttachShader(m_rendererId, vertexShader);
        glAttachShader(m_rendererId, fragmentShader);
        glLinkProgram(m_rendererId);

        verifyProgramLinked(m_rendererId);

        glDetachShader(m_rendererId, vertexShader);
        glDetachShader(m_rendererId, fragmentShader);
    }
    
    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_rendererId);
    }

    void OpenGLShader::bind() const {
        glUseProgram(m_rendererId);
    }

    void OpenGLShader::unbind() const {
        glUseProgram(0);
    }

    void OpenGLShader::setUniform(const std::string& uniformName, glm::mat4& uniform) const {
        unsigned int uniformLocation = glGetUniformLocation(m_rendererId, uniformName.c_str());
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(uniform));
    }

    void OpenGLShader::setUniform(const std::string& uniformName, glm::vec4& uniform) const {
        unsigned int uniformLocation = glGetUniformLocation(m_rendererId, uniformName.c_str());
        glUniform4fv(uniformLocation, 1, glm::value_ptr(uniform));
    }

    void OpenGLShader::setUniform(const std::string& uniformName, glm::vec3& uniform) const {
        unsigned int uniformLocation = glGetUniformLocation(m_rendererId, uniformName.c_str());
        glUniform3fv(uniformLocation, 1, glm::value_ptr(uniform));
    }

    unsigned int OpenGLShader::shaderDataTypeToNativeType(ShaderDataType type) {
        switch (type)
        {
            case ShaderDataType::VEC1:      return GL_FLOAT;
            case ShaderDataType::VEC2:      return GL_FLOAT;
            case ShaderDataType::VEC3:      return GL_FLOAT;
            case ShaderDataType::VEC4:      return GL_FLOAT;
            case ShaderDataType::MAT3:      return GL_FLOAT;
            case ShaderDataType::MAT4:      return GL_FLOAT;
            case ShaderDataType::IVEC1:     return GL_INT;
            case ShaderDataType::IVEC2:     return GL_INT;
            case ShaderDataType::IVEC3:     return GL_INT;
            case ShaderDataType::IVEC4:     return GL_INT;
            case ShaderDataType::BOOL:      return GL_BOOL;
            case ShaderDataType::NONE:      break;
        }

        BD_CORE_ERROR("Unknown Shader Data Type.");
        throw std::runtime_error("Unknown Shader Data Type.");
        return 0;
    }

}