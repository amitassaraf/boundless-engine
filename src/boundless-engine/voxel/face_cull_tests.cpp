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

cl_int findLocationalCodeIndex(const cl_ulong* octreeCodes, cl_uint totalNodes, cl_ulong locationalCode)
{
    cl_uint l = 0;
    while (l <= totalNodes) {
        cl_uint m = l + (totalNodes - l) / 2;
        if (octreeCodes[m] == locationalCode) 
            return static_cast<cl_int>(m);
        if (octreeCodes[m] < locationalCode) 
            l = m + 1;
        else
            totalNodes = m - 1; 
    }  
    return -1;
} 

cl_char isLeaf(cl_ulong* octreeCodes, cl_uint totalNodes, cl_ulong locationalCode) {
    if (findLocationalCodeIndex(octreeCodes, totalNodes, locationalCode << 3) == -1) {
        return TRUE;
    }
    return FALSE;
}

cl_char isSolid(const cl_uchar* octreeSolids, cl_ulong locationalCodeIndex) {
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
    return static_cast<cl_int>(static_cast<cl_float>(octreeSize) / pow(2.0f, (cl_float)getDepth(locationalCode)));
}

cl_char checkIfSiblingIsSolid(cl_ulong* octreeCodes, cl_uchar* octreeSolids, cl_int octreeSize, cl_uint totalNodes,
                           cl_ulong siblingLocationalCode, cl_ushort nodeSize,
                           cl_ulong faceBitsTestMask, cl_char expectingZeroResult, cl_uchar direction) {
    while (siblingLocationalCode > 1) {
        cl_int siblingLocationalCodeIndex = findLocationalCodeIndex(octreeCodes, totalNodes, siblingLocationalCode);
        if (siblingLocationalCodeIndex != -1) {
            cl_ulong sibling = siblingLocationalCode;
            if (isLeaf(octreeCodes, totalNodes, sibling) == FALSE) {
                // Find its smaller children that might hiding our face
                cl_ulong currentLocationalCode = sibling;

                // The following is an algorithm for iterating a Locational Code Octree system without recursion
                // and without a stack. (As both are not supported in OpenCL).
                // The constraints of the algorithm are bound by the depth maximum level of the Octree.
                // This theoretically can support 256 depth levels (Way more than needed) due to standard OpenCL 1.2
                // limitation of vectors size up to 16. Also when dealing with more than 21 depth levels, this
                // entire method / kernel should probably be rewritten to increase performance.

                // Using 2 x 64 bit numbers allows us to save up to 32 depth levels (We need 4 bits per level)
                // our game supports Octree Locational codes up to 64 bits (21 depth levels)
                // so this is more than enough.

                // We need 4 bits per level, because we need 0-7 to represent each node we left on, and 8 to represent
                // the end of a node. Sadly there is no way to do this with less than 4 bits per depth level.

                cl_ulong2 childrenStack = {{0, 0}};
                cl_char solidFlag = TRUE;

                while (currentLocationalCode != 0) {
                    // If the current node is not a leaf, dive into it's children.
                    if (isLeaf(octreeCodes, totalNodes, currentLocationalCode) == FALSE) {
                        // Find which child we left at previously (If any) using our childrenStack
                        cl_uchar currentDepth = getDepth(currentLocationalCode);
                        cl_uchar i;
                        cl_uchar divingIntoNode = FALSE;
                        // Choose which number in the stack we should address (each ulong supports 16 depth levels)
                        // >= 16 because first scalar is 0-15 depth, seconds scalar is 16-31 (32 depth levels).
                        if (currentDepth >= 16) {
                            currentDepth = currentDepth - 16;
                            i = (childrenStack.s[1] >> (currentDepth * 4)) & 0xf;
                            // Once we read from the stack, as a stack, we "pop" the bits by setting them to 0
                            childrenStack.s[1] = ((childrenStack.s[1] >> (currentDepth * 5)) << (currentDepth * 5)) | (childrenStack.s[1] & ((0x1 << (currentDepth * 4)) - 1));
                        } else {
                            i = (childrenStack.s[0] >> (currentDepth * 4)) & 0xf;
                            // Once we read from the stack, as a stack, we "pop" the bits by setting them to 0
                            childrenStack.s[0] = ((childrenStack.s[0] >> (currentDepth * 5)) << (currentDepth * 5)) | (childrenStack.s[0] & ((0x1 << (currentDepth * 4)) - 1));
                        }

                        // Iterate the children.
                        for (; i<8; i++) {
                            cl_ulong locCodeChild = (currentLocationalCode<<3)|i;

                            // Check if this child is relevant to examine (might not be in the direction we
                            // are coming to siblingLocationalCode or might be bigger)
                            // This is not part of the iteration algorithm.
                            if (isDirection(locCodeChild, direction) == FALSE || getSize(octreeSize, locCodeChild) > nodeSize) {
                                continue;
                            }

                            // Before diving into a child, save which child we left at in the childrenStack so when we
                            // iterate up again, we do are not stuck in an inifite loop and go over children
                            // we have previously visited
                            currentDepth = getDepth(currentLocationalCode);
                            if (currentDepth >= 16) {
                                currentDepth = currentDepth - 16;
                                childrenStack.s[1] = (((childrenStack.s[1] >> (currentDepth * 4)) | (i + 1)) << (currentDepth * 4)) | (childrenStack.s[1] & ((0x1 << (currentDepth * 4)) - 1));;
                            } else {
                                childrenStack.s[0] = (((childrenStack.s[0] >> (currentDepth * 4)) | (i + 1)) << (currentDepth * 4)) | (childrenStack.s[0] & ((0x1 << (currentDepth * 4)) - 1));;
                            }

                            // Dive into the child
                            currentLocationalCode = locCodeChild;
                            divingIntoNode = TRUE;
                            break;
                        }

                        if (divingIntoNode == TRUE) {
                            continue;
                        }

                        // Once we finish all the children for a node, we should reset it's children stack and move
                        // back up to it's parent. (We can reset the children stack by always XORing with 8 as the
                        // for loop will always end at 8.
                        currentDepth = getDepth(currentLocationalCode);
                        if (currentDepth >= 16) {
                            currentDepth = currentDepth - 16;
                            childrenStack.s[1] = ((childrenStack.s[1] >> (currentDepth * 5)) << (currentDepth * 5)) | (childrenStack.s[1] & ((0x1 << (currentDepth * 4)) - 1));;
                        } else {
                            childrenStack.s[0] = ((childrenStack.s[0] >> (currentDepth * 5)) << (currentDepth * 5)) | (childrenStack.s[0] & ((0x1 << (currentDepth * 4)) - 1));;
                        }

                        // Node is checked, go back to the parent.
                        currentLocationalCode = currentLocationalCode >> 3;

                        if (currentLocationalCode == sibling) {
                            currentLocationalCode = 0;
                            // Exit out of loop
                        }
                    } else {
                        // We have reached a leaf node, lets find if it fits our criteria then check if it is air
                        // This is not part of the iteration algorithm.
                        cl_uchar depth = getDepth(currentLocationalCode) - getDepth(siblingLocationalCode);
                        cl_ulong hyperLocalCode = ((currentLocationalCode >> (3 * depth)) << (3 * depth)) ^ currentLocationalCode;
                        cl_ulong hyperFaceBitsTestMask = faceBitsTestMask;
                        for (cl_uchar i = 0; i < depth - 1; i++) {
                            hyperFaceBitsTestMask = (hyperFaceBitsTestMask << 3) | faceBitsTestMask;
                        }

                        // If we find air, game's over. We can mark this face as solid and no need to continue iteration.
                        if (expectingZeroResult == TRUE) {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == hyperFaceBitsTestMask) {
                                if (isSolid(octreeSolids, findLocationalCodeIndex(octreeCodes, totalNodes, currentLocationalCode)) == FALSE) {
                                    solidFlag = FALSE;
                                    break;
                                }
                            }
                        } else {
                            if ((hyperLocalCode & hyperFaceBitsTestMask) == 0) {
                                if (isSolid(octreeSolids, findLocationalCodeIndex(octreeCodes, totalNodes, currentLocationalCode)) == FALSE) {
                                    solidFlag = FALSE;
                                    break;
                                }
                            }
                        }
                        // Back to the iteration algorithm
                        // Leaf is checked, go back to the parent.
                        currentLocationalCode = currentLocationalCode >> 3;
                    }
                }

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
        if (isLeaf(octreeCodes, totalNodes, locationalCode) == FALSE) {
            masks[locationalCodeIndex] = faceMask;
            continue;
        }

        if (isSolid(octreeSolids, locationalCodeIndex) == FALSE) {
            masks[locationalCodeIndex] = faceMask;
            continue;
        }

        BD_CORE_TRACE("Doing: {}", locationalCode);
        cl_ushort nodeSize = getSize(octreeSize, locationalCode);

        cl_ulong left;
        cl_ulong right;
        cl_ulong back;
        cl_ulong front;
        cl_ulong north;
        cl_ulong south;
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