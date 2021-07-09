#include "opencl_program.hpp"

namespace Boundless {

    OpenCLProgram::OpenCLProgram(const Ref<OpenCLContext> &context, const Ref<OpenCLDevice> &device,
                                 const std::string &shaderSrc, const std::string &entrypoint) {
        UNUSED(shaderSrc);

        cl_int error;

        // Compile the program
        const char *source = shaderSrc.c_str();
        this->m_program = clCreateProgramWithSource(context->getCLContext(), 1, (const char **) &source, nullptr,
                                                    &error);
        if (error != 0) {
            BD_CORE_ERROR("Error creating OpenCL program from source: {}", error);
        }

        error = clBuildProgram(this->m_program, 0, nullptr, nullptr, nullptr, nullptr);

        if (error == CL_BUILD_PROGRAM_FAILURE) {
            // Determine the size of the log
            size_t log_size;
            clGetProgramBuildInfo(this->m_program, device->getDeviceID(), CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);

            // Allocate memory for the log
            char *log = (char *) malloc(log_size);

            // Get the log
            clGetProgramBuildInfo(this->m_program, device->getDeviceID(), CL_PROGRAM_BUILD_LOG, log_size, log, nullptr);

            // Print the log
            BD_CORE_ERROR("Error in building program: {}", log);

            free(log);
            return;
        }

        this->m_kernel = clCreateKernel(this->m_program, entrypoint.c_str(), &error);
        if (error != 0) {
            BD_CORE_ERROR("Error creating OpenCL Kernel [{}]: {}", entrypoint, error);
        }
    }

    bool OpenCLProgram::addArgument(uint32_t index, const Ref<ComputeBuffer>& buffer) const {
        const Ref<OpenCLComputeBuffer> clBuffer = reinterpret_cast<const Ref<OpenCLComputeBuffer> &>(buffer);
        cl_mem clMemBuffer = clBuffer->getCLBuffer();
        cl_int error = clSetKernelArg(this->m_kernel, index, sizeof(cl_mem), &clMemBuffer);
        if (error != 0) {
            BD_CORE_ERROR("Error adding an OpenCL buffer argument to kernel {}", error);
        }
        return error != 0;
    }

    bool OpenCLProgram::addArgument(uint32_t index, size_t bytes, void* ptr) const {
        cl_int error = clSetKernelArg(this->m_kernel, index, bytes, ptr);
        if (error != 0) {
            BD_CORE_ERROR("Error adding an OpenCL argument to kernel {}", error);
        }
        return error != 0;
    }

    OpenCLProgram::~OpenCLProgram() {
        if (this->m_program) {
            clReleaseProgram(this->m_program);
        }
        if (this->m_kernel) {
            clReleaseKernel(this->m_kernel);
        }
    }

}