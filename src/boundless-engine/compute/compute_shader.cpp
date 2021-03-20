#include "compute_shader.hpp"
#include "platform/opencl/opencl_shader.hpp"
#include "compute_api.hpp"
#include <fstream>


namespace Boundless {

    ComputeShader::ComputeShader() {}
    ComputeShader::ComputeShader(const std::string& shaderSrc) {
        UNUSED(shaderSrc);
    }
    ComputeShader::~ComputeShader() {}


    ComputeShader* ComputeShader::create(const std::string& filepath) {
        std::ifstream shaderSourceFile( filepath );

        std::string shaderSrc( (std::istreambuf_iterator<char>(shaderSourceFile) ), (std::istreambuf_iterator<char>()    ) );

        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLShader(shaderSrc);
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}