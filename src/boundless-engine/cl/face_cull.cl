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

// leading zeros of 64 bit number
int __clzdi2(ulong val)
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

ulong calculateSibling(ulong startLocation, uchar mask, uchar whileThis, char backwardIsOr) {
        ulong target = startLocation;
        ulong path = 0;
        uchar count = 0;
        uchar latest = 0;
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

int findLocationalCodeIndex(__global ulong* octreeCodes, int totalNodes, ulong locationalCode) 
{ 
    int l = 0;
    while (l <= totalNodes) { 
        int m = l + (totalNodes - l) / 2; 
        if (octreeCodes[m] == locationalCode) 
            return m; 
        if (octreeCodes[m] < locationalCode) 
            l = m + 1;
        else
            totalNodes = m - 1; 
    }  
    return -1;
} 

char isLeaf(__global ulong* octreeCodes, int totalNodes, ulong locationalCode) {
    if (findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode << 3) == -1) {
        return TRUE;
    }
    return FALSE;
}

char isSolid(__global uchar* octreeSolids, ulong locationalCodeIndex) {
    if (octreeSolids[locationalCodeIndex] == 1) {
        return TRUE;
    }
    return FALSE;
}

char isDirection(uchar localCode, uchar direction) {
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

uchar getDepth(ulong locationalCode) {
    return (63-__clzdi2(locationalCode))/3;
}

int getSize(int octreeSize, ulong locationalCode) {
    return octreeSize / pow((float)2, (float)getDepth(locationalCode));
}

char visitAll(__global ulong* octreeCodes, __global uchar* octreeSolids, int octreeSize, int totalNodes, 
              ulong locationalCode, uchar direction, ushort nodeSize, ulong sibling, 
              ulong faceBitsTestMask, char expectingZeroResult) {
    if (isLeaf(octreeCodes, totalNodes, locationalCode) == FALSE) {
        for (int i=0; i<8; i++) {
            ulong locCodeChild = (locationalCode<<3)|i;

            if (isDirection(locationalCode, direction) == FALSE || getSize(octreeSize, locationalCode) > nodeSize) {
                return TRUE;
            }

            if (visitAll(octreeCodes, octreeSolids, octreeSize, totalNodes, locationalCode, direction, nodeSize, locCodeChild, faceBitsTestMask, expectingZeroResult) == FALSE) {
                return FALSE;
            }
        }
        return TRUE;
    }
                    
    uchar depth = getDepth(locationalCode) - getDepth(sibling);
    ulong hyperLocalCode = ((locationalCode >> (3 * depth)) << (3 * depth)) ^ locationalCode;
    ulong hyperFaceBitsTestMask = faceBitsTestMask;
    for (uchar i = 0; i < depth - 1; i++) {
        hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3) | faceBitsTestMask;
    }
    
    char solidFlag = TRUE;
    if (expectingZeroResult == TRUE) {
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


char checkIfSiblingIsSolid(__global ulong* octreeCodes, __global uchar* octreeSolids, int octreeSize, int totalNodes, 
                           ulong siblingLocationalCode, ushort nodeSize,
                           ulong faceBitsTestMask, char expectingZeroResult, uchar direction) {
    while (siblingLocationalCode > 1) {
        int siblingLocationalCodeIndex = findLocationalCodeIndex(octreeCodes, totalNodes, siblingLocationalCode);
        if (siblingLocationalCodeIndex != -1) {
            ulong sibling = siblingLocationalCode;
            if (isLeaf(octreeCodes, totalNodes, sibling) == FALSE) {
                // Find its smaller children that might hiding our face
                char solidFlag = visitAll(octreeCodes, octreeSolids, octreeSize, totalNodes, siblingLocationalCode, direction, nodeSize, sibling, faceBitsTestMask, expectingZeroResult);
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

__kernel void cullFaces(__global ulong* octreeCodes, __global uchar* octreeSolids, int octreeSize, int totalNodes, __global uchar* masks) {
    int wgId = get_group_id(0);
    int wgSize = get_local_size(0);
    int itemId = get_local_id(0);

    int startIndex = wgId * ELEMENTS_PER_GROUP;
    int endIndex = startIndex + ELEMENTS_PER_GROUP;
    if(endIndex > totalNodes){
        endIndex = totalNodes;
    }

    for(int locationalCodeIndex=startIndex + itemId; locationalCodeIndex<endIndex; locationalCodeIndex+= wgSize){
        ulong locationalCode = octreeCodes[locationalCodeIndex];

        uchar faceMask = 0;
        if (isLeaf(octreeCodes, totalNodes, locationalCode) == FALSE) {
            masks[locationalCodeIndex] = faceMask;
            continue;
        }

        if (isSolid(octreeSolids, locationalCodeIndex) == FALSE) {
            masks[locationalCodeIndex] = faceMask;
            continue;
        }
        
        ushort nodeSize = getSize(octreeSize, locationalCode);

        ulong left = 0;
        ulong right = 0;
        ulong back = 0;
        ulong front = 0;
        ulong north = 0;
        ulong south = 0;
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