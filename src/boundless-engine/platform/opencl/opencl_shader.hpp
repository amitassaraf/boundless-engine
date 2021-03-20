#pragma once
#ifndef BD_OPENCL_SHADER_HPP_
#define BD_OPENCL_SHADER_HPP_

#include "compute/compute_shader.hpp"
#include "logging/logger.hpp"
#include <vector>

#include <cl.hpp>

namespace Boundless {

    class OpenCLShader : public ComputeShader {
        public:
            OpenCLShader(const std::string& shaderSrc);
            virtual ~OpenCLShader();

            virtual void bind() const override;
            virtual void unbind() const override;
    };

}

#endif // !BD_OPENCL_SHADER_HPP_
