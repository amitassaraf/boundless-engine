#include "opencl_context.hpp"

namespace Boundless {

    OpenCLContext::OpenCLContext(const Ref<OpenCLDevice>& device) {
        cl_int error;

        // Create OpenCL context and command queue
        cl_device_id device_id = device->getDeviceID();
        this->m_context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &error);
        if (error != 0) {
            BD_CORE_ERROR("Error creating OpenCL context: {}", error);
        }
    }

    OpenCLContext::~OpenCLContext() {
        if (this->m_context) {
            clReleaseContext(this->m_context);
        }
    }

}