#include "compute_program.hpp"
#include "platform/opencl/opencl_program.hpp"
#include "compute_api.hpp"
#include <fstream>


namespace Boundless {

    ComputeProgram::ComputeProgram() = default;

    ComputeProgram::ComputeProgram(const Ref<ComputeContext> &context, const Ref<ComputeDevice> &device,
                                   const std::string &shaderSrc, const std::string &entrypoint) {
        UNUSED(shaderSrc);
        UNUSED(context);
        UNUSED(device);
        UNUSED(entrypoint);
    }

    ComputeProgram::~ComputeProgram() = default;


    ComputeProgram *ComputeProgram::create(const Ref<ComputeContext> &context, const Ref<ComputeDevice> &device,
                                           const std::string &filepath, const std::string &entrypoint) {
        std::ifstream shaderSourceFile(filepath);

        std::string shaderSrc((std::istreambuf_iterator<char>(shaderSourceFile)), (std::istreambuf_iterator<char>()));

        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLProgram(reinterpret_cast<const Ref<OpenCLContext> &>(context),
                                         reinterpret_cast<const Ref<OpenCLDevice> &>(device), shaderSrc, entrypoint);
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}