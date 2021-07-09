#pragma once
#ifndef BD_COMPUTE_DEVICE_HPP_
#define BD_COMPUTE_DEVICE_HPP_

#include <string>
#include "core/core.hpp"
#include "compute.fwd.hpp"

namespace Boundless {

    class ComputeDevice {
        public:
            ComputeDevice();
            virtual ~ComputeDevice();

            virtual bool getDeviceInformation(const Ref<ComputeProgram>& program, uint64_t param, size_t* ptr) const = 0;

            static ComputeDevice* create();
    };

}


#endif // !BD_COMPUTE_DEVICE_HPP_
