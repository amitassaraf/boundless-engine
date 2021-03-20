#include "opencl_shader.hpp"

namespace Boundless {

    OpenCLShader::OpenCLShader(const std::string& shaderSrc) {
        UNUSED(shaderSrc);

        // std::vector<cl::Platform> platforms;
        // cl::Platform::get(&platforms);

        // auto platform = platforms.front();

        // std::vector<cl::Device> devices;
        // platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

        // auto device = devices.front();

        // // Add the OpenCL source
        // cl::Program::Sources sources(1, std::make_pair(shaderSrc.c_str(), shaderSrc.length() + 1));
        // cl::Context context(device);
        // cl::Program program(context, sources);

        // cl_int err = program.build();

        // Split to class 
        // cl::Buffer memBuf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, sizeof(buf));

        // cl_int error;

        // // Split to class 
        // cl::Kernel kernel(program, "", &error);
        // kernel.setArg(0, memBuf);
        // cl::CommandQueue queue(context, device);
        // queue.enqueueTask(kernel);

        // queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);


    }
    
    OpenCLShader::~OpenCLShader() {

    }

    void OpenCLShader::bind() const {
    }

    void OpenCLShader::unbind() const {
    }

}