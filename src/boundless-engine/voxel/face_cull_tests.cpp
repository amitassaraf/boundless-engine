#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>

#define ELEMENTS_PER_GROUP 65536 //max # of elements to be processes by a group

#define FACE_TOP = 1; // 000
#define FACE_BOTTOM = 2; // 010
#define FACE_LEFT = 4; // 100
#define FACE_RIGHT = 8; // 1000
#define FACE_FRONT = 16;  // 10000
#define FACE_BACK = 32; // 100000

#define LEFT_RIGHT_FACE_BITS_TEST = 1;  // 0b001
#define FRONT_BACK_FACE_BITS_TEST = 2; // 0b010
#define TOP_BOTTOM_FACE_BITS_TEST = 4; // 0b100

// Leading zeros of 32 bit number
cl_int __clzsi2(cl_int a) {
  cl_uint x = (cl_uint)a;
  cl_int t = ((x & 0xFFFF0000) == 0) << 4;
  x >>= 16 - t;
  cl_uint r = t;
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

// leading zeros of 64 bit number
cl_int __clzdi2(cl_ulong val)
{
  if (val >> 32)
    {
      return __clzsi2(val >> 32);
    }
  else
    {
      return __clzsi2(val) + 32;
    }
}

cl_ulong calculateSibling(cl_ulong startLocation, cl_uchar mask, cl_uchar whileThis, cl_bool backwardIsOr) {
    cl_ulong target = startLocation;
    cl_ulong path = 0;
    cl_uchar count = 0;
    cl_uchar latest = 0;
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

cl_int findLocationalCodeIndex(cl_ulong* octreeCodes, cl_int totalNodes, cl_ulong locationalCode) 
{ 
    cl_int l = 0;
    while (l <= totalNodes) { 
        cl_int m = l + (totalNodes - l) / 2; 
        if (octreeCodes[m] == locationalCode) 
            return m; 
        if (octreeCodes[m] < locationalCode) 
            l = m + 1;
        else
            totalNodes = m - 1; 
    }  
    return -1;
} 

cl_bool isLeaf(cl_ulong* octreeCodes, cl_int totalNodes, cl_ulong locationalCode) {
    return findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode << 3) != -1;
}

cl_bool isSolid(cl_uchar* octreeSolids, cl_ulong locationalCodeIndex) {
    return octreeSolids[locationalCodeIndex] == 1;
}

cl_bool isDirection(cl_uchar localCode, cl_uchar direction) {
    if (direction == FACE_TOP) {
        return (localCode & 4) == 0;
    } else if (direction == FACE_BOTTOM) {
        return (localCode & 4) == 4;
    } else if (direction == FACE_FRONT) {
        return (localCode & 2) == 2;
    } else if (direction == FACE_BACK) {
        return (localCode & 2) == 0;
    } else if (direction == FACE_LEFT) {
        return (localCode & 1) == 1;
    } else if (direction == FACE_RIGHT) {
        return (localCode & 1) == 0;
    }
    return false;
}

cl_uchar getDepth(cl_ulong locationalCode) {
    return (63-__clzdi2(locationalCode))/3;
}

cl_int getSize(cl_int octreeSize, cl_ulong locationalCode) {
    return octreeSize / pow((cl_float)2, (cl_float)getDepth(locationalCode));
}

cl_bool visitAll(cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_int totalNodes, 
              cl_ulong locationalCode, cl_uchar direction, cl_ushort nodeSize, cl_ulong sibling, 
              cl_ulong faceBitsTestMask, cl_bool expectingZeroResult) {
    if (!isDirection(locationalCode, direction) || getSize(octreeSize, locationalCode) > nodeSize)
        return true;
    
    if (!isLeaf(octreeCodes, totalNodes, locationalCode)) {
        for (cl_int i=0; i<8; i++) {
            cl_ulong locCodeChild = (locationalCode<<3)|i;
            if (!visitAll(octreeCodes, octreeSolids, octreeSize, totalNodes, locationalCode, direction, nodeSize, locCodeChild, faceBitsTestMask, expectingZeroResult)) {
                return false;
            }
        }
        return true;
    }
                    
    cl_uchar depth = getDepth(locationalCode) - getDepth(sibling);
    cl_ulong hyperLocalCode = ((locationalCode >> (3 * depth)) << (3 * depth)) ^ locationalCode;
    cl_ulong hyperFaceBitsTestMask = faceBitsTestMask;
    for (cl_uchar i = 0; i < depth - 1; i++) {
        hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3) | faceBitsTestMask;
    }
    
    cl_bool solidFlag = true;
    if (expectingZeroResult) {
        if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
            if (!isSolid(octreeSolids, findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode))) {
                solidFlag = false;
            }
        }
    } else {
        if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
            if (!isSolid(octreeSolids, findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode))) {
                solidFlag = false;
            }
        }
    }

    return solidFlag;
}


cl_bool checkIfSiblingIsSolid(cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_int totalNodes, 
                           cl_ulong siblingLocationalCode, cl_ushort nodeSize,
                           cl_ulong faceBitsTestMask, cl_bool expectingZeroResult, cl_uchar direction) {
    while (siblingLocationalCode > 1) {
        cl_int siblingLocationalCodeIndex = findLocationalCodeIndex(octreeCodes, totalNodes, siblingLocationalCode);
        if (siblingLocationalCodeIndex != -1) {
            cl_ulong sibling = siblingLocationalCode;
            if (!isLeaf(octreeCodes, totalNodes, sibling)) {
                // Find its smaller children that might hiding our face
                cl_bool solidFlag = visitAll(octreeCodes, octreeSolids, octreeSize, totalNodes, siblingLocationalCode, direction, nodeSize, sibling, faceBitsTestMask, expectingZeroResult);
                if (!solidFlag) {
                    return false;
                }
            } else if (!isSolid(octreeSolids, siblingLocationalCodeIndex)) {
                return false;
            }

            return true;
        }
        siblingLocationalCode = siblingLocationalCode >> 3;
    }

    // Couldn't find surface
    return true;
}

void cullFaces(int wgId, int wgSize, int itemId, cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_int totalNodes, cl_uchar* masks) {
    int startIndex = wgId * ELEMENTS_PER_GROUP;
    int endIndex = startIndex + ELEMENTS_PER_GROUP;
    if(endIndex > n){
        endIndex = n;
    }

    for(cl_int locationalCodeIndex=startIndex + itemId; locationalCodeIndex<endIndex; locationalCodeIndex+= wgSize){
        cl_ulong locationalCode = octreeCodes[locationalCodeIndex];
        
        cl_uchar faceMask = 0;
        if (isLeaf(octreeCodes, totalNodes, locationalCodeIndex)) {
            masks[locationalCodeIndex] = faceMask;
            return;
        }

        if (!isSolid(octreeSolids, locationalCodeIndex)) {
            masks[locationalCodeIndex] = faceMask;
            return;
        }
        
        cl_ushort nodeSize = getSize(octreeSize, locationalCode);

        cl_ulong left = 0;
        cl_ulong right = 0;
        cl_ulong back = 0;
        cl_ulong front = 0;
        cl_ulong north = 0;
        cl_ulong south = 0;
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

        if (!checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, north, nodeSize, TOP_BOTTOM_FACE_BITS_TEST, false, FACE_TOP)) {
            faceMask |= FACE_TOP;
        }

        if (!checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, south, nodeSize, TOP_BOTTOM_FACE_BITS_TEST, true, FACE_BOTTOM)) {
            faceMask |= FACE_BOTTOM;
        }

        if (!checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, front, nodeSize, FRONT_BACK_FACE_BITS_TEST, true, FACE_FRONT)) {
            faceMask |= FACE_FRONT;
        }

        if (!checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, back, nodeSize, FRONT_BACK_FACE_BITS_TEST, false, FACE_BACK)) {
            faceMask |= FACE_BACK;
        }

        if (!checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, left, nodeSize, LEFT_RIGHT_FACE_BITS_TEST, true, FACE_LEFT)) {
            faceMask |= FACE_LEFT;
        }

        if (!checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, right, nodeSize, LEFT_RIGHT_FACE_BITS_TEST, false, FACE_RIGHT)) {
            faceMask |= FACE_RIGHT;
        }

        masks[locationalCodeIndex] = faceMask;          
    }
}

int main(int argc, char* argv) {
    
    for (int wgId = 0; wgId < totalItems / 256; wgId++) {
        for (int itemId = 0; itemId < 256; itemId++) {

        }
    }
    cullFaces();
}