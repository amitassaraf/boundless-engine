
uchar FACE_TOP = 1; // 000
uchar FACE_BOTTOM = 2; // 010
uchar FACE_LEFT = 4; // 100
uchar FACE_RIGHT = 8; // 1000
uchar FACE_FRONT = 16;  // 10000
uchar FACE_BACK = 32; // 100000

uchar LEFT_RIGHT_FACE_BITS_TEST = 1;  // 0b001
uchar FRONT_BACK_FACE_BITS_TEST = 2; // 0b010
uchar TOP_BOTTOM_FACE_BITS_TEST = 4; // 0b100

struct OctreeNode {
    ulong locationalCode;
}

int __clzsi2(int a) {
  uint x = (uint)a;
  int t = ((x & 0xFFFF0000) == 0) << 4;
  x >>= 16 - t;
  uint r = t;
  t = ((x & 0xFF00) == 0) << 3;
  x >>= 8 - t; 
  r += t;
  t = ((x & 0xF0) == 0) << 2;
  x >>= 4 - t; 
  r += t;      
  t = ((x & 0xC) == 0) << 1;
  x >>= 2 - t;
  r += t;
  return r + ((2 - x) & -((x & 2) == 0));
}

ulong calculateSibling(ulong startLocation, uchar mask, uchar whileThis, bool backwardIsOr) {
        ulong target = startLocation;
        ulong path = 0;
        uchar count = 0;
        uchar latest = 0;
        while ((target & mask) == whileThis && target > 1) {
            latest = target & 7;
            if (backwardIsOr) {
                path = path | ((latest | mask) << (3 * count));
            } else {
                path = path | ((latest ^ mask) << (3 * count));
            }
            target = target >> 3;
            count += 1;
        }
        if (target == 1) {
            return 1;
        } 
        
        if (backwardIsOr) {
            target = target ^ mask;
        } else {
            target = target | mask;
        }
        target = target << (3 * count);
        target = target | path;
        return target;
    }

int findLocationalCodeIndex(ulong* octree, int totalNodes, ulong locationalCode) 
{ 
    int l = 0;
    while (l <= totalNodes) { 
        int m = l + (totalNodes - l) / 2; 
        if (arr[m] == locationalCode) 
            return m; 
        if (arr[m] < locationalCode) 
            l = m + 1;
        else
            totalNodes = m - 1; 
    }  
    return -1;
} 


bool isDirection(uint8_t localCode, uint8_t direction) {
    switch (direction) {
        case FACE_TOP:
            return (localCode & 4u) == 0u;
        case FACE_BOTTOM:
            return (localCode & 4u) == 4u;
        case FACE_FRONT:
            return (localCode & 2u) == 2u;
        case FACE_BACK:
            return (localCode & 2u) == 0u;
        case FACE_LEFT:
            return (localCode & 1u) == 1u;
        case FACE_RIGHT:
            return (localCode & 1u) == 0u;
    }
    return false;
}

uchar getDepth(ulong locationalCode) {
    return (63-__clzsi2(locationalCode))/3;
}


void visitAll(ulong locationalCode, ) {
    if (!isDirection(locationalCode, direction) || !isLeaf(locationalCode) || node.getSize() > nodeSize)
        return;
                    
    uchar depth = node.getDepth() - sibling.getDepth();
    ulong hyperLocalCode = ((nodeLocationalCode >> (3u * depth)) << (3u * depth)) ^ nodeLocationalCode;
    ulong hyperFaceBitsTestMask = faceBitsTestMask;
    for (uchar i = 0u; i < depth - 1u; i++) {
        hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3u) | faceBitsTestMask;
    }
    
    if (expectingZeroResult) {
        if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
            if (!node.isSolid()) {
                solidFlag = false;
            }
        }
    } else {
        if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
            if (!node.isSolid()) {
                solidFlag = false;
            }
        }
    }

    if (isLeaf(locationalCode)) {
        return;
    }

    for (int i=0; i<8; i++) {
        ulong locCodeChild = (locationalCode<<3)|i;
        visitAll(locCodeChild);
    }
}


bool checkIfSiblingIsSolid(ulong* octreeCodes, ulong* octreeSolids, int totalNodes, ulong siblingLocationalCode, ushort nodeSize, ulong faceBitsTestMask, bool expectingZeroResult, uchar direction) {
    while (siblingLocationalCode > 1) {
        if (findLocationalCodeIndex(siblingLocationalCode) != -1) {
            ulong sibling = siblingLocationalCode;
            if (findLocationalCodeIndex(octreeCodes, totalNodes, sibling << 3) != -1) {
                // Find its smaller children that might hiding our face
                bool solidFlag = true;
                octree->visitAll(sibling, [&](ulong nodeLocationalCode, OctreeNode& node) {
                    
                    
                });
                if (!solidFlag) {
                    return false;
                }
            } else if (!sibling.isSolid()) {
                return false;
            }

            return true;
        }
        siblingLocationalCode = siblingLocationalCode >> 3;
    }

    // Couldn't find surface
    return true;
}

__kernel void cullFaces(__global ulong* octreeCodes, __global uchar* octreeSolids, __global uchar* octreeSizes, __global int totalNodes, __global uchar* masks) {
    int index = get_global_id(0);
    ulong locationalCode = octreeCodes[index];
    
    uchar faceMask = 0;
    if (findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode << 3) != -1) {
        masks[index] = faceMask;
        return;
    }

    if (octreeSolids[locationalCode] == 0) {
        masks[index] = faceMask;
        return;
    }

    ulong left = 0;
    ulong right = 0;
    ulong back = 0;
    ulong front = 0;
    ulong north = 0;
    ulong south = 0;
    if ((locationalCode & 4) == 4) { // Are we a top node
        north = calculateSibling(locationalCode, 4, 4, false);
        
        south = locationalCode ^ 4;
    } else { // We are a bottom node
        south = calculateSibling(locationalCode, 4, 0, true);
        
        north = locationalCode | 4;
    }
    // Get left and right
    if ((locationalCode & 1) == 1) { // Are we a right node?
        right = calculateSibling(locationalCode, 1, 1, false);

        left = locationalCode ^ 1;
    } else { // We are a right node
        left = calculateSibling(locationalCode, 1, 0, true);

        right = locationalCode | 1;
    }

    // Get back and front
    if ((locationalCode & 2) == 2) { // Are we a back node?
        back = calculateSibling(locationalCode, 2, 2, false);

        front = locationalCode ^ 2;
    } else { // We are a front node
        front = calculateSibling(locationalCode, 2, 0, true);

        back = locationalCode | 2;
    }

    if (!checkIfSiblingIsSolid(locationalCode, north, octreeSizes[index], TOP_BOTTOM_FACE_BITS_TEST, false, FACE_TOP)) {
        faceMask |= FACE_TOP;
    }

    if (!checkIfSiblingIsSolid(locationalCode, south, octreeSizes[index], TOP_BOTTOM_FACE_BITS_TEST, true, FACE_BOTTOM)) {
        faceMask |= FACE_BOTTOM;
    }

    if (!checkIfSiblingIsSolid(locationalCode, front, octreeSizes[index], FRONT_BACK_FACE_BITS_TEST, true, FACE_FRONT)) {
        faceMask |= FACE_FRONT;
    }

    if (!checkIfSiblingIsSolid(locationalCode, back, octreeSizes[index], FRONT_BACK_FACE_BITS_TEST, false, FACE_BACK)) {
        faceMask |= FACE_BACK;
    }

    if (!checkIfSiblingIsSolid(locationalCode, left, octreeSizes[index], LEFT_RIGHT_FACE_BITS_TEST, true, FACE_LEFT)) {
        faceMask |= FACE_LEFT;
    }

    if (!checkIfSiblingIsSolid(locationalCode, right, octreeSizes[index], LEFT_RIGHT_FACE_BITS_TEST, false, FACE_RIGHT)) {
        faceMask |= FACE_RIGHT;
    }

    masks[index] = faceMask;
}