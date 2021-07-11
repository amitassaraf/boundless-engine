#include <GL/glew.h>
#include <boundless.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm> 
#include <ThreadPool.h>
#include <thread>
#include <random>
#include "platform/opencl/opencl_program.hpp"
#include "platform/opencl/opencl_buffer.hpp"
#include "platform/opencl/opencl_context.hpp"
#include "platform/opencl/opencl_device.hpp"
#include "platform/opencl/opencl_command_queue.hpp"

class LeagueOfDwarves : public Boundless::Game {
public:
    std::vector<std::pair<Boundless::Ref<Boundless::VertexArray>, uint32_t> > m_toRender;
    Boundless::Ref<Boundless::Shader> m_shader;
    Boundless::Ref<Boundless::Shader> m_ssaoShader;
    Boundless::Ref<Boundless::Shader> m_ssaoBlurShader;
    Boundless::Ref<Boundless::Shader> m_ssaoLightingShader;
    Boundless::Ref<Boundless::PerspectiveCamera> m_camera;
    Boundless::Ref<Boundless::WindowLayer> m_windowLayer;
    Boundless::World world;
    Boundless::Scope<ThreadPool> m_pool;
    Boundless::Ref<Boundless::Texture> m_gPosition;
    Boundless::Ref<Boundless::Texture> m_gNormal;
    Boundless::Ref<Boundless::Texture> m_gAlbedo;
    std::vector<glm::vec3> m_ssaoNoise;
    Boundless::Ref<Boundless::Texture> m_noiseTexture;
    Boundless::Ref<Boundless::FrameBuffer> m_ssaoFBO;
    Boundless::Ref<Boundless::FrameBuffer> m_ssaoBlurFBO;
    Boundless::Ref<Boundless::FrameBuffer> m_gBuffer;
    Boundless::Ref<Boundless::RenderBuffer> m_renderBuffer;
    Boundless::Ref<Boundless::Texture> m_ssaoColorBuffer;
    Boundless::Ref<Boundless::Texture> m_ssaoColorBufferBlur;
    Boundless::Ref<Boundless::VertexArray> m_quad;
    std::vector<glm::vec3> m_ssaoKernel;
    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
    double previousTime = glfwGetTime();
    int frameCount = 0;

    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        m_windowLayer.reset(new Boundless::WindowLayer(m_eventManager));
        this->pushLayer(m_windowLayer.get());
        m_camera.reset(new Boundless::PerspectiveCamera(m_eventManager, m_windowLayer->getWidth(), m_windowLayer->getHeight()));
        m_camera->setPosition(glm::vec3(WORLD_SIZE / 2, WORLD_SIZE / 2, WORLD_SIZE / 2));
        m_pool.reset(new ThreadPool(std::thread::hardware_concurrency()));
        this->pushLayer(m_camera.get());
        this->pushLayer(new Boundless::FPSCounterLayer(m_eventManager));
    }

    ~LeagueOfDwarves() {

    }

    void prepareQuad() {
        float quadVertices[5 * 4] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };

        m_quad.reset(Boundless::VertexArray::create());
        Boundless::Ref<Boundless::VertexBuffer> m_vb;
        m_vb.reset(Boundless::VertexBuffer::create(quadVertices, sizeof(quadVertices)));
        Boundless::BufferLayout vertexLayout = {
            { Boundless::ShaderDataType::VEC3, "aPos" },
            { Boundless::ShaderDataType::VEC2, "aTexCoords" },  
        };
        m_vb->setLayout(vertexLayout);
        m_quad->addVertexBuffer(m_vb);


    }

    int findLocationalCodeIndex(std::vector<uint64_t> octreeCodes, int totalNodes, uint64_t locationalCode)
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

    void calcRenderNodes(Boundless::World& world) {
        BD_CORE_INFO("Traversing Meshes...");

        auto key_selector = [](auto pair){return pair.first;};
        std::vector<uint64_t> chunks(world.getOctree()->m_nodes.size());
        transform(world.getOctree()->m_nodes.begin(), world.getOctree()->m_nodes.end(), chunks.begin(), key_selector);

        m_toRender.clear();

        std::sort(chunks.begin(), chunks.end());

        BD_CORE_INFO("Calculating face masks...");

        float cubeVertices[3 * 8 * 2] = {
            0, 0, 0,   0, -1,  0,  // 0, nv front
            0, 0,  1,   0,  0,  1,  // 1, nv top
            1, 0, 0,   0,  0,  0,  // 2
            1, 0,  1,   1,  0,  0,  // 3, nv right
            1,  1, 0,   0,  1,  0,  // 4, nv back
            1,  1,  1,   0,  0,  0,  // 5
            0,  1, 0,   0,  0, -1,  // 6, nv bottom
            0,  1,  1,  -1,  0,  0,  // 7, nv left 
        };


        Boundless::Ref<Boundless::VertexBuffer> m_vb;
        m_vb.reset(Boundless::VertexBuffer::create(cubeVertices, sizeof(cubeVertices)));
        Boundless::BufferLayout vertexLayout = {
            { Boundless::ShaderDataType::VEC3, "v_Pos" },
            { Boundless::ShaderDataType::VEC3, "v_Normal" },  
        };
        m_vb->setLayout(vertexLayout);

        uint totalItems = world.getOctree()->m_nodes.size();
        std::vector<uint8_t> octreeSolids;
        octreeSolids.reserve(totalItems);

        int octreeSize = static_cast<int>(WORLD_SIZE);
        int totalNodes = static_cast<int>(totalItems);

        for (uint64_t code : chunks) {
            octreeSolids.push_back(world.getOctree()->m_nodes.at(code));
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
                chunks.data())
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

        if (maxWorkGroupSize > global) {
            maxWorkGroupSize = global;
        }

        computeCommands->enqueueTask(computeProgram, 1, 0, global, maxWorkGroupSize);
        computeCommands->flushCommands();

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_span = t2 - t1;
        BD_CORE_TRACE("It took: {} milliseconds for {} items.", time_span.count(), totalItems);

        uint8_t* results = new uint8_t[totalItems];

        computeCommands->enqueueRead(masks, true, 0, sizeof(uint8_t) * totalItems, results);
        computeCommands->flushCommands();

        std::unordered_map<uint8_t, std::vector<uint64_t> > maskToChunk;

        for (uint i = 0; i < totalItems; i++) {
            uint8_t mask = results[i];
            if (mask != 0 && (!world.getOctree()->nodeExists(chunks[i] << 3u) || world.getOctree()->m_nodes.at(chunks[i]) == 0u)) {
                maskToChunk[mask].push_back(chunks[i]);
            }
        }

        delete[] results;
        BD_CORE_INFO("Generating meshes...");

        for (int i = 1; i < 64; i++ ) {
            uint8_t faceMask = i;

            std::vector<float> cubePositions;
            uint32_t instanceCount = 0u;
            for (uint64_t chunkLocation : maskToChunk[faceMask]) {
                Boundless::OctreeNode chunk = world.getOctree()->getNodeAt(chunkLocation);
                glm::vec3 offset = chunk.getChunkOffset();
                cubePositions.push_back(offset.x);
                cubePositions.push_back(offset.y);
                cubePositions.push_back(offset.z);
                cubePositions.push_back((float)chunk.getSize());
                    
                instanceCount += 1u;
            }

            if (instanceCount == 0)
                continue;

            std::vector<uint32_t> cubeIndices;
            Boundless::Ref<Boundless::VertexBuffer> m_vbPositions;

            Boundless::Ref<Boundless::VertexArray> faceMesh = Boundless::Ref<Boundless::VertexArray>(Boundless::VertexArray::create());


            if ((faceMask & FACE_BOTTOM) == FACE_BOTTOM) {
                cubeIndices.insert(cubeIndices.end(), {0, 2, 3,   0, 3, 1});
            }

            if ((faceMask & FACE_TOP) == FACE_TOP) {
                cubeIndices.insert(cubeIndices.end(), {4, 6, 7,   4, 7, 5});
            }

            if ((faceMask & FACE_LEFT) == FACE_LEFT) {
                cubeIndices.insert(cubeIndices.end(), {6, 4, 2,   6, 2, 0});
            }

            if ((faceMask & FACE_RIGHT) == FACE_RIGHT) {
                cubeIndices.insert(cubeIndices.end(), {1, 3, 5,   1, 5, 7});
            }
            
            if ((faceMask & FACE_FRONT) == FACE_FRONT) {
                cubeIndices.insert(cubeIndices.end(), {3, 2, 4,   3, 4, 5});
            }

            if ((faceMask & FACE_BACK) == FACE_BACK) {
                
                cubeIndices.insert(cubeIndices.end(), {7, 6, 0,   7, 0, 1});
            }

            float* positions = &cubePositions[0];
            m_vbPositions.reset(Boundless::VertexBuffer::create(positions, cubePositions.size() * sizeof(float)));
            Boundless::BufferLayout layout = {
                { Boundless::ShaderDataType::VEC3, "m_Translation", true },
                { Boundless::ShaderDataType::VEC1, "m_Scale", true },
            };
            m_vbPositions->setLayout(layout);


            faceMesh->addVertexBuffer(m_vb);
            faceMesh->addVertexBuffer(m_vbPositions, 2);


            uint32_t* indicies = &cubeIndices[0];
            Boundless::Ref<Boundless::IndexBuffer> m_ib;
            m_ib.reset(Boundless::IndexBuffer::create(indicies, cubeIndices.size()));
            faceMesh->setIndexBuffer(m_ib);

            if (instanceCount > 0u) {
                m_toRender.push_back(std::make_pair(faceMesh, instanceCount));
            }
        }
    }

    float lerp(float a, float b, float f) {
        return a + f * (b - a);
    }  

    void initialize() override {
        world.generateWorld();

        calcRenderNodes(world);

        m_shader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl/world"));
        m_ssaoShader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl/world_ssao"));
        m_ssaoBlurShader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl/world_ssao_blur"));
        m_ssaoLightingShader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl/world_lighting"));

        m_eventManager.appendListener(Boundless::EventType::KEY_PRESSED, [&](const Boundless::Ref<Boundless::Event> event) {
            Boundless::Ref<Boundless::KeyPressedEvent> keyPressedEvent = std::dynamic_pointer_cast<Boundless::KeyPressedEvent> (event);
            
            if (keyPressedEvent->getKeyCode() == 84) {
                m_pool->enqueue([&]() { 
                    world.renderWorldAround(m_camera->getPosition());
                });
                
            } else if (keyPressedEvent->getKeyCode() == 82) {
                calcRenderNodes(world);
            }
        });

        prepareQuad();

        m_gBuffer.reset(Boundless::FrameBuffer::create());
        m_gBuffer->bind();

        // Prepare m_gBuffer
        m_gPosition.reset(Boundless::Texture::create2DTexture(m_windowLayer->getWidth(), m_windowLayer->getHeight(), Boundless::TextureColorChannel::RGBA, Boundless::TextureColorChannel::RGBA16F, Boundless::TextureDataType::FLOAT, NULL));
        m_gPosition->bind();
        m_gPosition->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        m_gPosition->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        m_gPosition->setTextureParameter(Boundless::TextureParameterName::WRAP_S, Boundless::TextureParameter::CLAMP_TO_EDGE);
        m_gPosition->setTextureParameter(Boundless::TextureParameterName::WRAP_T, Boundless::TextureParameter::CLAMP_TO_EDGE);
        m_gBuffer->set2DTexture(0, m_gPosition);

        m_gNormal.reset(Boundless::Texture::create2DTexture(m_windowLayer->getWidth(), m_windowLayer->getHeight(), Boundless::TextureColorChannel::RGBA, Boundless::TextureColorChannel::RGBA16F, Boundless::TextureDataType::FLOAT, NULL));
        m_gNormal->bind();
        m_gNormal->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        m_gNormal->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        m_gBuffer->set2DTexture(1, m_gNormal);

        m_gAlbedo.reset(Boundless::Texture::create2DTexture(m_windowLayer->getWidth(), m_windowLayer->getHeight(), Boundless::TextureColorChannel::RGBA, Boundless::TextureColorChannel::RGBA, Boundless::TextureDataType::UNSIGNED_BYTE, NULL));
        m_gAlbedo->bind();
        m_gAlbedo->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        m_gAlbedo->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        m_gBuffer->set2DTexture(2, m_gAlbedo);
        
        unsigned int indexes[3] = { 0, 1, 2 };
        m_gBuffer->enableTextureIndexes(indexes, 3);

        m_renderBuffer.reset(Boundless::RenderBuffer::create(Boundless::RenderBufferType::DEPTH_BUFFER, m_windowLayer->getWidth(), m_windowLayer->getHeight()));
        m_gBuffer->setRenderBuffer(Boundless::FrameBufferAttachmentType::DEPTH, m_renderBuffer);
        m_gBuffer->unbind();
        // END OF Prepare m_gBuffer


        // Prepare SSAO FBO
        m_ssaoFBO.reset(Boundless::FrameBuffer::create());
        m_ssaoBlurFBO.reset(Boundless::FrameBuffer::create());

        m_ssaoFBO->bind();
        m_ssaoColorBuffer.reset(Boundless::Texture::create2DTexture(m_windowLayer->getWidth(), m_windowLayer->getHeight(), Boundless::TextureColorChannel::RED, Boundless::TextureColorChannel::RED, Boundless::TextureDataType::FLOAT, NULL));
        m_ssaoColorBuffer->bind();
        m_ssaoColorBuffer->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        m_ssaoColorBuffer->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        m_ssaoFBO->set2DTexture(0, m_ssaoColorBuffer);
        
        m_ssaoBlurFBO->bind();

        m_ssaoColorBufferBlur.reset(Boundless::Texture::create2DTexture(m_windowLayer->getWidth(), m_windowLayer->getHeight(), Boundless::TextureColorChannel::RED, Boundless::TextureColorChannel::RED, Boundless::TextureDataType::FLOAT, NULL));
        m_ssaoColorBufferBlur->bind();
        m_ssaoColorBufferBlur->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        m_ssaoColorBufferBlur->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        m_ssaoBlurFBO->set2DTexture(0, m_ssaoColorBufferBlur);
        m_ssaoBlurFBO->unbind();
        // END OF Prepare SSAO FBO

        // Generate sample kernel
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaoKernel;
        for (unsigned int i = 0; i < 32; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 32.0;

            // scale samples s.t. they're more aligned to center of kernel
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            m_ssaoKernel.push_back(sample);
        }
        
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::mat3 rmat = glm::rotate(randomFloats(generator) * glm::pi<float>() * 2.0f, glm::vec3(0, 0, 1));
            glm::vec3 noise = rmat * glm::vec3(1, 0, 0);
            m_ssaoNoise.push_back(noise);
        }

        m_noiseTexture.reset(Boundless::Texture::create2DTexture(4, 4, Boundless::TextureColorChannel::RGB, Boundless::TextureColorChannel::RGBA32F, Boundless::TextureDataType::FLOAT, &m_ssaoNoise[0]));
        m_noiseTexture->bind();
        m_noiseTexture->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        m_noiseTexture->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        m_noiseTexture->setTextureParameter(Boundless::TextureParameterName::WRAP_S, Boundless::TextureParameter::REPEAT);
        m_noiseTexture->setTextureParameter(Boundless::TextureParameterName::WRAP_T, Boundless::TextureParameter::REPEAT);
        m_noiseTexture->unbind();

    }

    void onUpdate() override {
        double currentTime = glfwGetTime();
        frameCount++;

        // glm::mat4 model = glm::mat4(1.0f);
        Boundless::Renderer::beginScene();

        Boundless::RenderCommand::setClearColor({ 0.196078f, 0.6f, 0.8f, 1.0f });
        Boundless::RenderCommand::clear();
        
        // 1. Draw geometry into the gBuffer
        m_gBuffer->bind();
        Boundless::RenderCommand::clear();
        m_shader->bind();

        m_shader->setUniform("view", m_camera->getViewMatrix());
        m_shader->setUniform("projection", m_camera->getProjectionMatrix());
        
        Boundless::RenderCommand::fillMode();

        for (auto pair : m_toRender) {
            pair.first->bind();
            Boundless::Renderer::submitInstanced(pair.first, pair.second);
        }
        
        m_gBuffer->unbind();
        
        // 2. generate SSAO texture
        m_ssaoFBO->bind();
        m_windowLayer->updateViewport();
        Boundless::RenderCommand::clearColor();
        m_ssaoShader->bind();
        m_ssaoShader->setUniform("gPosition", 0);
        m_ssaoShader->setUniform("gNormal", 1);
        m_ssaoShader->setUniform("noiseTexture", 2);
        m_ssaoShader->setUniform("screenDimensions", glm::vec2(m_windowLayer->getWidth(), m_windowLayer->getHeight()));
        // Send kernel + rotation to shader
        for (unsigned int i = 0; i < 32; ++i) {
            m_ssaoShader->setUniform("samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);
        }
        m_ssaoShader->setUniform("projection", m_camera->getProjectionMatrix());
        m_ssaoShader->setUniform("view", m_camera->getViewMatrix());
        m_ssaoShader->setActiveTextureUnit(0);
        m_gPosition->bind();
        m_ssaoShader->setActiveTextureUnit(1);
        m_gNormal->bind();
        m_ssaoShader->setActiveTextureUnit(2);
        m_noiseTexture->bind();
        // Render the SSAO texture to Quad
        m_quad->bind();
        Boundless::Renderer::submit(m_quad);
        m_quad->unbind();
        m_ssaoFBO->unbind();



        m_ssaoBlurFBO->bind();
        Boundless::RenderCommand::clearColor();
        m_ssaoBlurShader->bind();
        m_ssaoBlurShader->setUniform("ssaoInput", 0);
        m_ssaoBlurShader->setActiveTextureUnit(0);
        m_ssaoColorBuffer->bind();
        m_quad->bind();
        Boundless::Renderer::submit(m_quad);
        m_quad->unbind();
        m_ssaoBlurFBO->unbind();
        

        // 3. lighting pass
        Boundless::RenderCommand::clear();
        m_ssaoLightingShader->bind();
        m_ssaoLightingShader->setUniform("gPosition", 0);
        m_ssaoLightingShader->setUniform("gNormal", 1);
        m_ssaoLightingShader->setUniform("gAlbedo", 2);
        m_ssaoLightingShader->setUniform("ssao", 3);
        

        glm::vec3 lightPosView = glm::vec3(m_camera->getViewMatrix() * glm::vec4(lightPos, 1.0));
        m_ssaoLightingShader->setUniform("light.Position", lightPosView);
        m_ssaoLightingShader->setUniform("light.Color", lightColor);
        // Update attenuation parameters
        const float linear    = 0.09;
        const float quadratic = 0.032;
        m_ssaoLightingShader->setUniform("light.Linear", linear);
        m_ssaoLightingShader->setUniform("light.Quadratic", quadratic);

        m_ssaoLightingShader->setActiveTextureUnit(0);
        m_gPosition->bind();
        m_ssaoLightingShader->setActiveTextureUnit(1);
        m_gNormal->bind();
        m_ssaoLightingShader->setActiveTextureUnit(2);
        m_gAlbedo->bind();
        m_ssaoLightingShader->setActiveTextureUnit(3);
        m_ssaoColorBufferBlur->bind();
        // Render the SSAO texture to Quad
        m_quad->bind();
        Boundless::Renderer::submit(m_quad);
        m_quad->unbind();

        Boundless::Renderer::endScene();
        if ( currentTime - previousTime >= 1.0 ) {
            // Display the frame count here any way you want.
            BD_CORE_TRACE("FPS: {}", frameCount);

            frameCount = 0;
            previousTime = currentTime;
        }

    }


};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
