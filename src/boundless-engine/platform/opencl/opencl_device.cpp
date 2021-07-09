#include "opencl_device.hpp"
#include "opencl_program.hpp"

namespace Boundless {

    cl_device_id OpenCLDevice::chooseDevice() const {
        cl_device_id device_ids[MAX_DEVICES_PER_SYSTEM];

        // Get all device IDs in the system
        clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, MAX_DEVICES_PER_SYSTEM, device_ids, nullptr);

        cl_device_id bestDevice;
        cl_uint bestPerformance = 0;
        for (auto deviceId : device_ids) {
            if (deviceId) {
                cl_uint computeUnits;
                cl_uint clockFrequency;

                clGetDeviceInfo(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFrequency), &clockFrequency,
                                nullptr);
                clGetDeviceInfo(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, nullptr);

                if (bestPerformance < computeUnits * clockFrequency) {
                    bestPerformance = computeUnits * clockFrequency;
                    bestDevice = deviceId;
                }
            }
        }

        cl_char device_name[MAX_DEVICE_NAME_LENGTH] = {0};
        clGetDeviceInfo(bestDevice, CL_DEVICE_NAME, sizeof(device_name), &device_name, nullptr);
        BD_CORE_TRACE("Compute Device: {}", device_name);

        return bestDevice;
    }

    bool OpenCLDevice::getDeviceInformation(const Ref<ComputeProgram> &program, uint64_t param, size_t *ptr) const {
        const Ref<OpenCLProgram> clProgram = reinterpret_cast<const Ref<OpenCLProgram> &>(program);
        cl_int error = clGetKernelWorkGroupInfo(clProgram->getCLKernel(), this->m_deviceId, param, sizeof(size_t), ptr,
                                                nullptr);
        if (error != 0) {
            BD_CORE_ERROR("Error enqueuing read in command queque: {}", error);
        }
        return error != 0;
    }

    OpenCLDevice::OpenCLDevice() {
        // Get the best device from the system
        this->m_deviceId = this->chooseDevice();
    }

    OpenCLDevice::~OpenCLDevice() {

    }

}