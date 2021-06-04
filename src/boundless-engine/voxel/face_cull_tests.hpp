#pragma once
#ifndef BD_FACE_CULL_TESTS_HPP_
#define BD_FACE_CULL_TESTS_HPP_

#include "core/core.hpp"

#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>
#include "logging/logger.hpp"

namespace Boundless {
    void cullFaces(cl_uint wgId, cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_uint totalNodes, cl_uchar* masks);
}


#endif // !BD_FACE_CULL_TESTS_HPP_
