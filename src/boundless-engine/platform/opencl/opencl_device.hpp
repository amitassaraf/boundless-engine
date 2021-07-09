#pragma once
#ifndef BD_OPENCL_DEVICE_HPP_
#define BD_OPENCL_DEVICE_HPP_

#include "compute/compute_device.hpp"
#include "logging/logger.hpp"
#include <vector>

#include <cl.hpp>

namespace Boundless {

    static const int MAX_DEVICE_NAME_LENGTH = 1024;
    const uint8_t MAX_DEVICES_PER_SYSTEM = 2;

    class OpenCLDevice : public ComputeDevice {
        public:
            OpenCLDevice();
            ~OpenCLDevice() override;

            [[nodiscard]] cl_device_id chooseDevice() const;
            bool getDeviceInformation(const Ref<ComputeProgram>& program, uint64_t param, size_t* ptr) const override;

            [[nodiscard]] cl_device_id getDeviceID() const { return m_deviceId; }
        private:
            cl_device_id m_deviceId;

    };

}

#endif // !BD_OPENCL_DEVICE_HPP_
