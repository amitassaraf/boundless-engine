#include "voxel/octree_node.hpp"
#include "core/core.hpp"
#include "logging/logger.hpp"
#include <bitset>

namespace Boundless {

    uint16_t calculateOffset(uint64_t mask, uint64_t starter, uint16_t base, uint8_t bitOffset) {
        return ((mask & starter) >> (2 - bitOffset) << base) // 0
        + ((mask & (starter) << 3) >> (4 - bitOffset) << base) // 1
        + ((mask & (starter) << 6) >> (6 - bitOffset) << base) // 2
        + ((mask & (starter) << 9) >> (8 - bitOffset) << base) // 3
        + ((mask & (starter) << 12) >> (10 - bitOffset) << base) // 4
        + ((mask & (starter) << 15) >> (12 - bitOffset) << base) // 5
        + ((mask & (starter) << 18) >> (14 - bitOffset) << base) // 6
        + ((mask & (starter) << 21) >> (16 - bitOffset) << base) // 7
        + ((mask & (starter) << 24) >> (18 - bitOffset) << base) // 8
        + ((mask & (starter) << 27) >> (20 - bitOffset) << base) // 9
        + ((mask & (starter) << 30) >> (22 - bitOffset) << base) // 10
        + ((mask & (starter) << 33) >> (24 - bitOffset) << base) // 11
        + ((mask & (starter) << 36) >> (26 - bitOffset) << base) // 12
        + ((mask & (starter) << 39) >> (28 - bitOffset) << base) // 13
        + ((mask & (starter) << 42) >> (30 - bitOffset) << base) // 14
        + ((mask & (starter) << 45) >> (32 - bitOffset) << base) // 15
        + ((mask & (starter) << 48) >> (34 - bitOffset) << base) // 16
        + ((mask & (starter) << 51) >> (36 - bitOffset) << base) // 17
        + ((mask & (starter) << 54) >> (38 - bitOffset) << base) // 18
        + ((mask & (starter) << 57) >> (40 - bitOffset) << base) // 19
        + ((mask & (starter) << 60) >> (42 - bitOffset)); // 21 Cannot leftshift node size
    }

    glm::vec3 OctreeNode::getChunkOffset(std::uint64_t locationalCode, std::uint16_t octreeSize) {
        uint16_t nodeSize = log2(getSize(locationalCode, octreeSize));
        uint64_t tops = locationalCode & BIT64_TOP_BOTTOM_FACE_BITS_TEST;
        uint64_t rights = locationalCode & BIT64_LEFT_RIGHT_FACE_BITS_TEST;
        uint16_t lsb = __builtin_clzll(locationalCode);
        uint64_t fronts = ((locationalCode ^ BIT64_FRONT_BACK_FACE_BITS_TEST) << lsb) >> lsb & BIT64_FRONT_BACK_FACE_BITS_TEST;

        return glm::vec3(calculateOffset(fronts, 0x2u, nodeSize, 1u), 
                         calculateOffset(tops, 0x4u, nodeSize, 0u), 
                         calculateOffset(rights, 0x1u, nodeSize, 2u) - octreeSize);
    }

    uint64_t OctreeNode::getPackedChunk(std::uint64_t locationalCode, std::uint16_t octreeSize) {
        uint16_t nodeSize = log2(getSize(locationalCode, octreeSize));
        uint64_t tops = locationalCode & BIT64_TOP_BOTTOM_FACE_BITS_TEST;
        uint64_t rights = locationalCode & BIT64_LEFT_RIGHT_FACE_BITS_TEST;
        uint16_t lsb = __builtin_clzll(locationalCode);
        uint64_t fronts = ((locationalCode ^ BIT64_FRONT_BACK_FACE_BITS_TEST) << lsb) >> lsb & BIT64_FRONT_BACK_FACE_BITS_TEST;

        uint64_t packed = (((((calculateOffset(fronts, 0x2u, nodeSize, 1u) << 16)
                        | calculateOffset(rights, 0x1u, nodeSize, 2u) - octreeSize) << 16)
                        | calculateOffset(tops, 0x4u, nodeSize, 0u)) << 16) | nodeSize;
        return packed;
    }


    std::uint16_t OctreeNode::getSize(std::uint64_t locationalCode, std::uint16_t octreeSize) {
        return octreeSize / pow(2u, getDepth(locationalCode));
    }

    std::size_t OctreeNode::getDepth(std::uint64_t locationalCode) {
        if (locationalCode == 0) {
            throw std::runtime_error("No depth.");
        }

    #if defined(__GNUC__)
        return (63-__builtin_clzll(locationalCode))/3;
    #elif defined(_MSC_VER)
        long msb;
        _BitScanReverse(&msb, m_locationalCode);
        return msb/3;
    #endif
    }

}