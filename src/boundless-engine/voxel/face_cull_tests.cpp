#include "face_cull_tests.hpp"
#define ELEMENTS_PER_GROUP 65536 //max # of elements to be processes by a group

#define FACE_TOP 1 // 000
#define FACE_BOTTOM 2 // 010
#define FACE_LEFT 4 // 100
#define FACE_RIGHT 8 // 1000
#define FACE_FRONT 16  // 10000
#define FACE_BACK 32 // 100000

#define LEFT_RIGHT_FACE_BITS_TEST 1  // 0b001
#define FRONT_BACK_FACE_BITS_TEST 2 // 0b010
#define TOP_BOTTOM_FACE_BITS_TEST 4 // 0b100
#define TRUE 1
#define FALSE 0

// Leading zeros of 32 bit number
cl_int clzsi2(cl_int a) {
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
cl_int clzdi2(cl_ulong value) {
  if (value >> 32)
    {
      return clzsi2(value >> 32);
    }
  else
    {
      return clzsi2(value) + 32;
    }
}

cl_ulong calculateSibling(cl_ulong startLocation, cl_uchar mask, cl_uchar whileThis, cl_char backwardIsOr) {
        cl_ulong target = startLocation;
        cl_ulong path = 0;
        cl_uchar count = 0;
        cl_uchar latest = 0;
        while ((target & mask) == whileThis && target > 1) {
            latest = target & 7;
            if (backwardIsOr == TRUE) {
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
        
        if (backwardIsOr == TRUE) {
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

cl_char isLeaf(cl_ulong* octreeCodes, int totalNodes, cl_ulong locationalCode) {
    if (findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode << 3) != -1) {
        return TRUE;
    }
    return FALSE;
}

cl_char isSolid(cl_uchar* octreeSolids, cl_ulong locationalCodeIndex) {
    if (octreeSolids[locationalCodeIndex] == 1) {
        return TRUE;
    }
    return FALSE;
}

cl_char isDirection(cl_uchar localCode, cl_uchar direction) {
    if (direction == FACE_TOP) {
        if ((localCode & 4) == 0) {
            return TRUE;
        }
        return FALSE;
    } else if (direction == FACE_BOTTOM) {
        if ((localCode & 4) == 4) {
            return TRUE;
        }
        return FALSE;
    } else if (direction == FACE_FRONT) {
        if ((localCode & 2) == 2) {
            return TRUE;
        }
        return FALSE;
    } else if (direction == FACE_BACK) {
        if ((localCode & 2) == 0) {
            return TRUE;
        }
        return FALSE;
    } else if (direction == FACE_LEFT) {
        if ((localCode & 1) == 1) {
            return TRUE;
        }
        return FALSE;
    } else if (direction == FACE_RIGHT) {
        if ((localCode & 1) == 0) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

cl_uchar getDepth(cl_ulong locationalCode) {
    return (63-clzdi2(locationalCode))/3;
}

cl_int getSize(cl_int octreeSize, cl_ulong locationalCode) {
    return octreeSize / pow((cl_float)2, (cl_float)getDepth(locationalCode));
}

cl_char visitAll(cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_int totalNodes, 
              cl_ulong locationalCode, cl_uchar direction, cl_ushort nodeSize, cl_ulong sibling, 
              cl_ulong faceBitsTestMask, cl_char expectingZeroResult) {
    if (isDirection(locationalCode, direction) == FALSE || getSize(octreeSize, locationalCode) > nodeSize)
        return TRUE;
    
    if (isLeaf(octreeCodes, totalNodes, locationalCode) == FALSE) {
        for (cl_int i=0; i<8; i++) {
            cl_ulong locCodeChild = (locationalCode<<3)|i;
            if (visitAll(octreeCodes, octreeSolids, octreeSize, totalNodes, locCodeChild, direction, nodeSize, locationalCode, faceBitsTestMask, expectingZeroResult) == FALSE) {
                return FALSE;
            }
        }
        return TRUE;
    }
                    
    cl_uchar depth = getDepth(locationalCode) - getDepth(sibling);
    cl_ulong hyperLocalCode = ((locationalCode >> (3 * depth)) << (3 * depth)) ^ locationalCode;
    cl_ulong hyperFaceBitsTestMask = faceBitsTestMask;
    for (cl_uchar i = 0; i < depth - 1; i++) {
        hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3) | faceBitsTestMask;
    }
    
    cl_char solidFlag = TRUE;
    if (expectingZeroResult) {
        if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
            if (isSolid(octreeSolids, findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode)) == FALSE) {
                solidFlag = FALSE;
            }
        }
    } else {
        if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
            if (isSolid(octreeSolids, findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode)) == FALSE) {
                solidFlag = FALSE;
            }
        }
    }

    return solidFlag;
}


cl_char checkIfSiblingIsSolid(cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_int totalNodes, 
                           cl_ulong siblingLocationalCode, cl_ushort nodeSize,
                           cl_ulong faceBitsTestMask, char expectingZeroResult, cl_uchar direction) {
    while (siblingLocationalCode > 1) {
        cl_int siblingLocationalCodeIndex = findLocationalCodeIndex(octreeCodes, totalNodes, siblingLocationalCode);
        if (siblingLocationalCodeIndex != -1) {
            cl_ulong sibling = siblingLocationalCode;
            if (isLeaf(octreeCodes, totalNodes, sibling) == FALSE) {
                // Find its smaller children that might hiding our face
                cl_char solidFlag = visitAll(octreeCodes, octreeSolids, octreeSize, totalNodes, sibling, direction, nodeSize, siblingLocationalCode, faceBitsTestMask, expectingZeroResult);
                if (solidFlag == FALSE) {
                    return FALSE;
                }
            } else if (isSolid(octreeSolids, siblingLocationalCodeIndex) == FALSE) {
                return FALSE;
            }

            return TRUE;
        }
        siblingLocationalCode = siblingLocationalCode >> 3;
    }

    // Couldn't find surface
    return TRUE;
}

namespace Boundless {

void cullFaces(cl_uint wgId, cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_uint totalNodes, cl_uchar* masks) {
    cl_uint startIndex = wgId * ELEMENTS_PER_GROUP;
    cl_uint endIndex = startIndex + ELEMENTS_PER_GROUP;
    if(endIndex > totalNodes){
        endIndex = totalNodes;
    }

    for(cl_uint locationalCodeIndex=startIndex; locationalCodeIndex<endIndex; locationalCodeIndex++){
        cl_ulong locationalCode = octreeCodes[locationalCodeIndex];

        cl_uchar faceMask = 0;
        if (isLeaf(octreeCodes, totalNodes, locationalCodeIndex) == TRUE) {
            masks[locationalCodeIndex] = faceMask;
            continue;
        }

        if (isSolid(octreeSolids, locationalCodeIndex) == FALSE) {
            masks[locationalCodeIndex] = faceMask;
            continue;
        }
        
        cl_ushort nodeSize = getSize(octreeSize, locationalCode);

        cl_ulong left = 0;
        cl_ulong right = 0;
        cl_ulong back = 0;
        cl_ulong front = 0;
        cl_ulong north = 0;
        cl_ulong south = 0;
        if ((locationalCode & 4) == 4) { // Are we a top node
            north = calculateSibling(locationalCode, 4, 4, FALSE);
            
            south = locationalCode ^ 4;
        } else { // We are a bottom node
            south = calculateSibling(locationalCode, 4, 0, TRUE);
            
            north = locationalCode | 4;
        }
        // Get left and right
        if ((locationalCode & 1) == 1) { // Are we a right node?
            right = calculateSibling(locationalCode, 1, 1, FALSE);

            left = locationalCode ^ 1;
        } else { // We are a right node
            left = calculateSibling(locationalCode, 1, 0, TRUE);

            right = locationalCode | 1;
        }

        // Get back and front
        if ((locationalCode & 2) == 2) { // Are we a back node?
            back = calculateSibling(locationalCode, 2, 2, FALSE);

            front = locationalCode ^ 2;
        } else { // We are a front node
            front = calculateSibling(locationalCode, 2, 0, TRUE);

            back = locationalCode | 2;
        }

        if (checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, north, nodeSize, TOP_BOTTOM_FACE_BITS_TEST, FALSE, FACE_TOP) == FALSE) {
            faceMask |= FACE_TOP;
        }

        if (checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, south, nodeSize, TOP_BOTTOM_FACE_BITS_TEST, TRUE, FACE_BOTTOM) == FALSE) {
            faceMask |= FACE_BOTTOM;
        }

        if (checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, front, nodeSize, FRONT_BACK_FACE_BITS_TEST, TRUE, FACE_FRONT) == FALSE) {
            faceMask |= FACE_FRONT;
        }

        if (checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, back, nodeSize, FRONT_BACK_FACE_BITS_TEST, FALSE, FACE_BACK) == FALSE) {
            faceMask |= FACE_BACK;
        }

        if (checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, left, nodeSize, LEFT_RIGHT_FACE_BITS_TEST, TRUE, FACE_LEFT) == FALSE) {
            faceMask |= FACE_LEFT;
        }

        if (checkIfSiblingIsSolid(octreeCodes, octreeSolids, octreeSize, totalNodes, right, nodeSize, LEFT_RIGHT_FACE_BITS_TEST, FALSE, FACE_RIGHT) == FALSE) {
            faceMask |= FACE_RIGHT;
        }

        masks[locationalCodeIndex] = faceMask;          
    }
}

}