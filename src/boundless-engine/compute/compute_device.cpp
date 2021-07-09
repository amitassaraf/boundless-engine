#include "compute_device.hpp"
#include "platform/opencl/opencl_device.hpp"
#include "compute_api.hpp"


namespace Boundless {

    ComputeDevice::ComputeDevice() {}

    ComputeDevice::~ComputeDevice() {}


    ComputeDevice *ComputeDevice::create() {

        switch (Compute::getApi()) {
            case ComputeAPI::NONE:
                BD_CORE_ERROR("Compute API None is not supported");
                throw std::runtime_error("Compute API None is not supported.");
                break;
            case ComputeAPI::OPEN_CL:
                return new OpenCLDevice();
                break;
        }

        BD_CORE_ERROR("No Compute API was found");
        throw std::runtime_error("No Compute API was found");
        return nullptr;
    }

}