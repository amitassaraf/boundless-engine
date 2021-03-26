#pragma once
#ifndef BD_COMPUTE_API_HPP_
#define BD_COMPUTE_API_HPP_

#include <memory>
#include "core/core.hpp"

namespace Boundless {


    enum class ComputeAPI {
        NONE = 0,
        OPEN_CL,
    };

    class Compute {
        public:
            virtual void clear() = 0;
            
            inline static ComputeAPI getApi() { return s_Api; }
        private:
            static ComputeAPI s_Api;
    };

}

#endif // !BD_COMPUTE_API_HPP_
