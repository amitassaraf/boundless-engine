#pragma once
#ifndef BD_COMPUTE_PROGRAM_HPP_
#define BD_COMPUTE_PROGRAM_HPP_

#include <string>
#include "core/core.hpp"
#include "compute.fwd.hpp"
#include "compute_context.hpp"
#include "buffer.hpp"

namespace Boundless {

    class ComputeProgram {
    public:
        ComputeProgram();

        ComputeProgram(const Ref<ComputeContext> &context, const Ref<ComputeDevice> &device,
                       const std::string &shaderSrc, const std::string &entrypoint);

        virtual ~ComputeProgram();

        virtual bool addArgument(uint32_t index, const Ref<ComputeBuffer>& buffer) const = 0;
        virtual bool addArgument(uint32_t index, size_t bytes, void* ptr) const = 0;

        static ComputeProgram* create(const Ref<ComputeContext> &context, const Ref<ComputeDevice> &device, const std::string &filepath,
               const std::string &entrypoint);
    };

}


#endif // !BD_COMPUTE_PROGRAM_HPP_
