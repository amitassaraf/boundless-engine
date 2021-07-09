#include "opencl_command_queue.hpp"

namespace Boundless {

    OpenCLCommandQueue::OpenCLCommandQueue(const Ref<OpenCLContext> &context, const Ref<OpenCLDevice> &device) {
        cl_int error;

        cl_device_id device_id = device->getDeviceID();
        this->m_commandQueue = clCreateCommandQueue(context->getCLContext(), device_id, 0, &error);
        if (error != 0) {
            BD_CORE_ERROR("Error creating OpenCL command queue: {}", error);
        }

    }

    bool OpenCLCommandQueue::enqueueTask(const Ref<ComputeProgram> &program, uint8_t dimensions, size_t offset,
                                         size_t globalSize, size_t localSize) const {
        const Ref<OpenCLProgram> clProgram = reinterpret_cast<const Ref<OpenCLProgram> &>(program);
        cl_int error = clEnqueueNDRangeKernel(this->m_commandQueue, clProgram->getCLKernel(), dimensions,
                                              &offset, &globalSize, &localSize, 0, nullptr, nullptr);
        if (error != 0) {
            BD_CORE_ERROR("Error enqueuing task in command queque: {}", error);
        }
        return error != 0;
    }

    bool
    OpenCLCommandQueue::enqueueRead(const Ref<ComputeBuffer> &buffer, bool blocking, size_t offset, size_t bytes,
                                    void *ptr) const {
        const Ref<OpenCLComputeBuffer> clBuffer = reinterpret_cast<const Ref<OpenCLComputeBuffer> &>(buffer);
        cl_int error = clEnqueueReadBuffer(this->m_commandQueue, clBuffer->getCLBuffer(), blocking ? CL_TRUE : CL_FALSE,
                                           offset, bytes, ptr, 0, nullptr, nullptr);
        if (error != 0) {
            BD_CORE_ERROR("Error enqueuing read in command queque: {}", error);
        }
        return error != 0;
    }

    void OpenCLCommandQueue::flushCommands() const {
        cl_int error = clFinish(this->m_commandQueue);
        if (error != 0) {
            BD_CORE_ERROR("Error flushing OpenCL command queue: {}", error);
        }
    }

    OpenCLCommandQueue::~OpenCLCommandQueue() {
        if (this->m_commandQueue) {
            clReleaseCommandQueue(this->m_commandQueue);
        }
    }

}