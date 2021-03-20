#pragma once
#ifndef BD_COMPUTER_SHADER_HPP_
#define BD_COMPUTER_SHADER_HPP_

#include <string>
#include "core/core.hpp"

namespace Boundless {

    class ComputeShader {
        public:
            ComputeShader();
            ComputeShader(const std::string& shaderSrc);
            virtual ~ComputeShader();

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            
            static ComputeShader* create(const std::string& filepath);
        protected:
            uint32_t m_computeId;
    };

}


#endif // !BD_COMPUTER_SHADER_HPP_
