#include "shader.hpp"

namespace Boundless {

    Shader::Shader() {}
    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        UNUSED(vertexSrc);
        UNUSED(fragmentSrc);
    }
    Shader::~Shader() {}

}