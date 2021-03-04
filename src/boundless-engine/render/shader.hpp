#pragma once
#ifndef BD_SHADER_HPP_
#define BD_SHADER_HPP_

#include <string>
#include "core/core.hpp"

namespace Boundless {

    class Shader {
        public:
            Shader();
            Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
            virtual ~Shader();

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
        protected:
            uint32_t m_rendererId;
    };

}

#endif // !BD_SHADER_HPP_
