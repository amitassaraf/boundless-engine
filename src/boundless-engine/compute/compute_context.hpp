#pragma once
#ifndef BD_COMPUTE_CONTEXT_HPP_
#define BD_COMPUTE_CONTEXT_HPP_

#include <string>
#include "core/core.hpp"
#include "compute_device.hpp"

namespace Boundless {

    class ComputeContext {
        public:
            ComputeContext();
            ComputeContext(const Ref<ComputeDevice>& device);
            virtual ~ComputeContext();

            static ComputeContext* create(const Ref<ComputeDevice>& device);
    };

}


#endif // !BD_COMPUTE_CONTEXT_HPP_
