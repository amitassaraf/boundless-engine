#include "voxel/world.hpp"
#include "core/core.hpp"
#include <glm/vec3.hpp>
#include "logging/logger.hpp"
#include "platform/opencl/opencl_program.hpp"
#include "platform/opencl/opencl_buffer.hpp"
#include "platform/opencl/opencl_context.hpp"
#include "platform/opencl/opencl_device.hpp"
#include "platform/opencl/opencl_command_queue.hpp"
#include <bitset>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>
#include <ratio>
#include <chrono>
#include "face_cull_tests.hpp"

#define __CL_ENABLE_EXCEPTIONS

#include <OpenCL/opencl.h>

#define WORLD_SIZE 1024

float scale = 90.f;
float lacunarity = 0.9f;
float persistance = 1.3f;

const int octaves = static_cast<int>(3 + std::log(scale)); // Estimate number of octaves needed for the current scale

const float min = -1.0f;
const float max = 1.0f;

float normalize(float input) {
    float average = (min + max) / 2.0f;
    float range = (max - min) / 2.0f;
    float normalized_x = (input - average) / range;
    return (normalized_x + 1.0f) / 2.0f;
}

int noise[WORLD_SIZE][WORLD_SIZE];

namespace Boundless {

    World::World() : m_noise(SimplexNoise(0.1f / scale, 0.5f, lacunarity, persistance)) {
        m_size = WORLD_SIZE;
        m_octree.reset(new Octree(m_size));

        for (int x = 0; x < WORLD_SIZE; x++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                noise[x][z] = floor(normalize(m_noise.fractal(octaves, x, z)) * m_size);
            }
        }

    }

    World::~World() {
    }

    int World::shouldDivide(const glm::vec3 &chunkOffset, uint16_t nodeSize) {
        int above = 1;

        int step = 1;

        if (nodeSize > 64) {
            step = round(nodeSize / 10);
        }

        for (int x = chunkOffset.x; x < chunkOffset.x + nodeSize; x += step) {
            for (int z = chunkOffset.z; z < chunkOffset.z + nodeSize; z += step) {
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

        renderWorldAround(glm::vec3(WORLD_SIZE / 2, WORLD_SIZE / 2, WORLD_SIZE / 2));

        BD_CORE_INFO("Running OpenCL test...");

        uint totalItems = m_octree->m_nodes.size();
        std::vector<cl_ulong> octreeCodes;
        octreeCodes.reserve(totalItems);
        std::vector<cl_uchar> octreeSolids;
        octreeSolids.reserve(totalItems);

        cl_int octreeSize = static_cast<cl_int>(WORLD_SIZE);
        cl_int totalNodes = static_cast<cl_int>(totalItems);

        for (auto kv : m_octree->m_nodes) {
            octreeCodes.push_back(static_cast<cl_ulong>(kv.first));
        }

        std::sort(octreeCodes.begin(), octreeCodes.end());

        for (cl_ulong code : octreeCodes) {
            uint8_t solid = m_octree->m_nodes.at(code);
            octreeSolids.push_back(static_cast<cl_uchar>(solid));
        }

        BD_CORE_INFO("Running Culling OpenCL...");

        Boundless::Ref<Boundless::ComputeDevice> computeDevice;
        computeDevice.reset(Boundless::ComputeDevice::create());

        Boundless::Ref<Boundless::ComputeContext> computeContext;
        computeContext.reset(Boundless::ComputeContext::create(computeDevice));

        Boundless::Ref<Boundless::ComputeCommandQueue> computeCommands;
        computeCommands.reset(Boundless::ComputeCommandQueue::create(computeContext, computeDevice));

        Boundless::Ref<Boundless::ComputeProgram> computeProgram;
        computeProgram.reset(Boundless::ComputeProgram::create(
                computeContext,
                computeDevice,
                "/Users/amitassaraf/workspace/league_of_dwarves/src/boundless-engine/cl/face_cull.cl",
                "cullFaces")
        );

        Boundless::Ref<Boundless::ComputeBuffer> octreeCodesBuffer;
        octreeCodesBuffer.reset(Boundless::ComputeBuffer::create(
                computeContext,
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                totalItems * sizeof(cl_ulong),
                octreeCodes.data())
        );

        Boundless::Ref<Boundless::ComputeBuffer> octreeSolidsBuffer;
        octreeSolidsBuffer.reset(Boundless::ComputeBuffer::create(
                computeContext,
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                totalItems * sizeof(cl_uchar),
                octreeSolids.data())
        );

        Boundless::Ref<Boundless::ComputeBuffer> masks;
        masks.reset(Boundless::ComputeBuffer::create(
                computeContext,
                CL_MEM_READ_WRITE,
                totalItems * sizeof(cl_uchar),
                nullptr)
        );

        computeCommands->flushCommands();

        computeProgram->addArgument(0, octreeCodesBuffer);
        computeProgram->addArgument(1, octreeSolidsBuffer);
        computeProgram->addArgument(2, sizeof(cl_int), &octreeSize);
        computeProgram->addArgument(3, sizeof(cl_int), &totalNodes);
        computeProgram->addArgument(4, masks);

        size_t maxWorkGroupSize;
        computeDevice->getDeviceInformation(computeProgram, CL_KERNEL_WORK_GROUP_SIZE, &maxWorkGroupSize);

        BD_CORE_TRACE("Max Global WS: {}", maxWorkGroupSize);

        size_t global = totalItems / maxWorkGroupSize;

        // Change global to the nearest power of 2
        TRANSFORM_16_BIT_NEAREST_POWER_OF_TWO(global)

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        BD_CORE_TRACE("Local Work Group: {}, Global Work Items: {}", maxWorkGroupSize, global);

        computeCommands->enqueueTask(computeProgram, 1, 0, global, maxWorkGroupSize);
        computeCommands->flushCommands();

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_span = t2 - t1;
        BD_CORE_TRACE("It took: {} milliseconds for {} items.", time_span.count(), totalItems);

        cl_uchar results[totalItems];

        computeCommands->enqueueRead(masks, true, 0, sizeof(cl_uchar) * totalItems, results);
        computeCommands->flushCommands();

        BD_CORE_INFO("Checking Culling OpenCL...");


        for (uint i = 0; i < totalItems; i++) {
            cl_ulong locationalCode = octreeCodes[i];
            uint8_t mask = m_octree->calculateFaceMask(locationalCode);
            cl_uchar outMask = results[i];
            if (mask != outMask) {
                BD_CORE_TRACE("MISMATCH {}: {}vs{}", locationalCode, mask, outMask);
            }
        }

        BD_CORE_INFO("Done!");
    }

    OctreeNode World::findIntersectingNode(const glm::vec3 &position) {
        OctreeNode rootNode = m_octree->getRootNode();
        uint64_t closestNode = 1u;
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
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

    void World::renderWorldAround(const glm::vec3 &playerPosition) {
        glm::vec3 camera = playerPosition;

        BD_CORE_INFO("Calculating LODs...");
        OctreeNode rootNode = m_octree->getRootNode();
        m_octree->visitAllConditional(rootNode, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
            UNUSED(nodeLocationalCode);

            glm::vec3 chunkLocation = node.getChunkOffset();
            uint16_t size = node.getSize();
            glm::vec3 chunkCenter(chunkLocation.x + (size / 2.0f), chunkLocation.y + (size / 2.0f),
                                  chunkLocation.z + (size / 2.0f));
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

    bool World::collapseNode(OctreeNode &lodNode) {
        bool solid = false;
        m_octree->visitAllBottomUp(lodNode, [&](uint64_t nodeLocationalCode, OctreeNode &node) {
            UNUSED(nodeLocationalCode);

            if (node.isSolid()) {
                solid = true;
            }
            m_octree->erase(nodeLocationalCode);
        });
        m_octree->setNodeAt(lodNode.getLocationalCode(), solid ? 1u : 0u);
        return true;
    }

    bool World::divideNode(OctreeNode &lodNode, const glm::vec3 &referenceOffset) {
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