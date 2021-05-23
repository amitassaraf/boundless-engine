#include "voxel/world.hpp"
#include "core/core.hpp"
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include <bitset>
#include <glm/gtx/string_cast.hpp>
#include <algorithm> 
#include <fstream>

#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>

float scale     = 90.f;
float lacunarity    = 0.9f;
float persistance   = 1.3f;

const int octaves = static_cast<int>(3 + std::log(scale)); // Estimate number of octaves needed for the current scale

const float min = -1.0f;
const float max = 1.0f;

float normalize(float input)
{
    float average      = (min + max) / 2.0f;
    float range        = (max - min) / 2.0f;
    float normalized_x = (input - average) / range;
    return (normalized_x + 1.0f) / 2.0f;
}
        
int noise[1024][1024];

namespace Boundless {

    World::World() : m_noise(SimplexNoise(0.1f/scale, 0.5f, lacunarity, persistance)) {
        m_size = 1024u;
        m_octree.reset(new Octree(m_size));

        for (int x = 0; x < 1024; x++) {
            for (int z = 0; z < 1024; z++) {
                noise[x][z] = floor(normalize(m_noise.fractal(octaves, x, z)) * m_size);
            }     
        }
        
    }

    World::~World() {
    }

    int World::shouldDivide(const glm::vec3& chunkOffset, uint16_t nodeSize) {
        int above = 1;

        int step = 1;
        
        if (nodeSize > 64) {
            step = round(nodeSize / 10);    
        }
        
        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x+=step) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z+=step) {
                float normalized = noise[x][z];
                if (nodeSize > 1.0f && normalized >= chunkOffset.y && normalized < chunkOffset.y + nodeSize) {
                    return 0;
                } else if (normalized >= chunkOffset.y + nodeSize) {
                    above = 1;
                } else if (normalized < chunkOffset.y) {
                    above = -1;
                }
            }
        }
        return above;
    }

    void World::generateWorld() {
        OctreeNode rootNode = m_octree->getRootNode();
        m_octree->divide(rootNode);
        BD_CORE_INFO("Generating world...");

        renderWorldAround(glm::vec3(512,512,512));

        BD_CORE_INFO("Running openCL...");


        std::ifstream shaderSourceFile( "../cl/face_cull.cl" );

        std::string shaderSrc( (std::istreambuf_iterator<char>(shaderSourceFile) ), (std::istreambuf_iterator<char>()    ) );

        int err;
        cl_device_id device_id;
        clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
        cl_context context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
        cl_command_queue commands = clCreateCommandQueue(context, device_id, 0, &err);
        const char* source = shaderSrc.c_str();
        cl_program program = clCreateProgramWithSource(context, 1, (const char **) & source, NULL, &err);

        clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
        cl_kernel kernel = clCreateKernel(program, "cullFaces", &err);
        
        uint totalItems = m_octree->m_nodes.size();
        std::vector<cl_ulong> keys;
        keys.reserve(totalItems);
        std::vector<cl_uchar> vals;
        vals.reserve(totalItems);        

        for(auto kv : m_octree->m_nodes) {
            keys.push_back(static_cast<cl_ulong>(kv.first));
            vals.push_back(static_cast<cl_uchar>(kv.second));  
        } 
        
        cl_mem octreeCodes = clCreateBuffer(context,  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,  totalItems * sizeof(cl_ulong), NULL, NULL);
        cl_mem octreeSolids = clCreateBuffer(context,  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,  totalItems * sizeof(cl_uchar), NULL, NULL);
        cl_mem masks = clCreateBuffer(context,  CL_MEM_WRITE_ONLY,  totalItems * sizeof(cl_uchar), NULL, NULL);
        // Split to class 

        err = clEnqueueWriteBuffer(commands, octreeCodes, CL_TRUE, 0, totalItems * sizeof(cl_ulong), keys.data(), 0, NULL, NULL);
        err = clEnqueueWriteBuffer(commands, octreeSolids, CL_TRUE, 0, totalItems * sizeof(cl_uchar), vals.data(), 0, NULL, NULL);
        
        cl_int octreeSize = static_cast<cl_int>(1024);
        cl_int totalNodes = static_cast<cl_int>(totalItems);
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &octreeCodes);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &octreeSolids);
        clSetKernelArg(kernel, 2, sizeof(cl_int), &octreeSize);
        clSetKernelArg(kernel, 3, sizeof(cl_int), &totalNodes);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), &masks);

        size_t local = 256;
        // clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
        size_t global = totalItems;
        BD_CORE_TRACE("LOCAL: {}, GLOBAL: {}", local, global);
        clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
        
        clFinish(commands);

        cl_uchar results[totalItems];
        clEnqueueReadBuffer(commands, masks, CL_TRUE, 0, sizeof(cl_uchar) * totalItems, results, 0, NULL, NULL);
 
        clFinish(commands);

        for (uint i = 0; i < totalItems; i++) {
            if (results[i] != 0) {
                BD_CORE_TRACE("MASK: {}", results[i]);
            }
        }

        clReleaseMemObject(octreeCodes);
        clReleaseMemObject(octreeSolids);
        clReleaseMemObject(masks);
        clReleaseProgram(program);
        clReleaseKernel(kernel);
        clReleaseCommandQueue(commands);
        clReleaseContext(context);

        BD_CORE_INFO("Done...");
    }

    OctreeNode World::findIntersectingNode(const glm::vec3& position) {
        OctreeNode rootNode = m_octree->getRootNode();
        uint64_t closestNode = 1u;
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode& node) {
            glm::vec3 chunkLocation = node.getChunkOffset();
            uint16_t size = node.getSize();

            if (chunkLocation.x >= position.x && chunkLocation.x + size < position.x) {
                if (chunkLocation.z >= position.z && chunkLocation.z + size < position.z) {
                    if (chunkLocation.y >= position.y && chunkLocation.y + size < position.y) {
                        closestNode = nodeLocationalCode;
                        return true;
                    }
                }    
            }

            return false;
        });

        return m_octree->getNodeAt(closestNode);
    }

    void World::renderWorldAround(const glm::vec3& playerPosition) {
        glm::vec3 camera = playerPosition;

        BD_CORE_INFO("Calculating LODs...");
        OctreeNode rootNode = m_octree->getRootNode();
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode& node) {
            UNUSED(nodeLocationalCode);

            glm::vec3 chunkLocation = node.getChunkOffset();
            uint16_t size = node.getSize();
            glm::vec3 chunkCenter(chunkLocation.x + (size / 2.0f), chunkLocation.y + (size / 2.0f), chunkLocation.z + (size / 2.0f));
            auto distance = abs(glm::length(camera - chunkCenter));

            if (distance < (size * 250)) {
                if (size > 1 && m_octree->isLeaf(node)) {
                    divideNode(node, chunkLocation);
                }

                return true;
            } else if (!m_octree->isLeaf(node) && size < m_octree->m_size) {
                collapseNode(node);
            }
            return false;
        });

        BD_CORE_INFO("Done!");
    }

    bool World::collapseNode(OctreeNode& lodNode) {
        bool solid = false;
        m_octree->visitAllBottomUp(lodNode, [&](uint64_t nodeLocationalCode, OctreeNode& node) {
            UNUSED(nodeLocationalCode);
            
            if (node.isSolid()) {
                solid = true;
            }
            m_octree->erase(nodeLocationalCode);
        });
        m_octree->setNodeAt(lodNode.getLocationalCode(), solid ? 1u : 0u);
        return true;
    }

    bool World::divideNode(OctreeNode& lodNode, const glm::vec3& referenceOffset) {
        int aboveBelowOrDivide = this->shouldDivide(referenceOffset, lodNode.getSize());
        if (aboveBelowOrDivide == 0) {
            m_octree->divide(lodNode);
            m_octree->setNodeAt(lodNode.getLocationalCode(), 0u);
            return true;
        } else if (aboveBelowOrDivide == 1) {
            m_octree->setNodeAt(lodNode.getLocationalCode(), 1u);
        } else if (aboveBelowOrDivide == -1) {
            m_octree->setNodeAt(lodNode.getLocationalCode(), 0u);
        }
        return false;
    }

}