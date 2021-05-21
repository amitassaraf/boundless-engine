int BIT64_LEFT_RIGHT_FACE_BITS_TEST = 0x9249249249249249; // 0b001001001001001001001001001001
int BIT64_FRONT_BACK_FACE_BITS_TEST = 0x2492492492492492; // 0b010010010010010010010010010010
int BIT64_TOP_BOTTOM_FACE_BITS_TEST = 0x4924924924924924; // 0b100100100100100100100100100100
const sampler_t sampler=CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

int calculateOffset(int mask, int starter, int base, int bitOffset) {
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

int3 getNodeOffset(int locationalCode, int octreeSize, int nodeSize) {
    int logerithmicSize = log2(nodeSize);
    int tops = locationalCode & BIT64_TOP_BOTTOM_FACE_BITS_TEST;
    int rights = locationalCode & BIT64_LEFT_RIGHT_FACE_BITS_TEST;
    int lsb = __builtin_clzll(locationalCode);
    int fronts = ((m_locationalCode ^ BIT64_FRONT_BACK_FACE_BITS_TEST) << lsb) >> lsb & BIT64_FRONT_BACK_FACE_BITS_TEST;

    return int3(calculateOffset(fronts, 2, logerithmicSize, 1), 
                    calculateOffset(tops, 4, logerithmicSize, 0), 
                    calculateOffset(rights, 1, logerithmicSize, 2) - octreeSize);
}

int shouldDivide(int nodeSize, int3 nodeOffset, image2d_t noise) {
    int step = 1;
    
    if (nodeSize > 64) {
        step = round(nodeSize / 10);    
    }
    
    for (int x = nodeOffset.x; x < nodeOffset.x + nodeSize; x+=step) {
        for (int z = nodeOffset.z; z < nodeOffset.z + nodeSize; z+=step) {
            float4 normalized = read_imagef(noise, sampler, (int2)(x, z));
            if (nodeSize > 1.0f && normalized >= nodeOffset.y && normalized < nodeOffset.y + nodeSize) {
                return 0;
            } else if (normalized >= nodeOffset.y + nodeSize) {
                above = 1;
            } else if (normalized < nodeOffset.y) {
                above = -1;
            }
        }
    }

    return above;
}

__kernel void octreeLODGeneration(__read_only image2d_t noise, int nodeSize, __global int octreeSize, __global int* octree, __global int* solidOctree) {
    int index = get_global_id(0);
    int locationalCode = octree[index];
    int3 nodeOffset = getNodeOffset(locationalCode, octreeSize, nodeSize);

    int above = shouldDivide(nodeSize, nodeOffset, noise);

    if (above == 1) {
        // Add to array as is
        solidOctree[index] = locationalCode;
    } else if (above == 0) {
        // Divide
        for (int i=0; i<8; i++) {
            int childLocationalCode = (locationalCode << 3) | i;
            octree[index + i] = childLocationalCode;
        }
    }
}