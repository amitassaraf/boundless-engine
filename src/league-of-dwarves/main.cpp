#include <boundless.h>
#include <glm/gtx/string_cast.hpp>
#include <algorithm> 
#include <ThreadPool.h>
#include <thread>
#include <random>

class LeagueOfDwarves : public Boundless::Game {
public:
    std::vector<std::pair<Boundless::Ref<Boundless::VertexArray>, uint32_t> > m_toRender;
    Boundless::Ref<Boundless::Shader> m_shader;
    Boundless::Ref<Boundless::PerspectiveCamera> m_camera;
    Boundless::Ref<Boundless::LocatedUniform> view;
    Boundless::Ref<Boundless::LocatedUniform> viewPos;
    Boundless::Ref<Boundless::LocatedUniform> projection;
    Boundless::World world;
    Boundless::Scope<ThreadPool> m_pool;

    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
        m_camera.reset(new Boundless::PerspectiveCamera(m_eventManager));
        m_pool.reset(new ThreadPool(std::thread::hardware_concurrency()));
        this->pushLayer(m_camera.get());
        this->pushLayer(new Boundless::FPSCounterLayer(m_eventManager));
    }

    ~LeagueOfDwarves() {

    }


    void calcRenderNodes(Boundless::World& world) {
        BD_CORE_INFO("Traversing Meshes...");

        std::vector<uint64_t> chunks;
        int nodes = 0;
        m_toRender.clear();
        Boundless::OctreeNode root = world.getOctree()->getRootNode();
        world.getOctree()->visitAll(root, [&](uint64_t nodeLocationalCode, Boundless::OctreeNode& node) {
            nodes += 1;
            UNUSED(nodeLocationalCode);

            if (!world.getOctree()->isLeaf(node) || !node.isSolid()) {
                return;
            }

            chunks.push_back(nodeLocationalCode);
        });
        
        BD_CORE_INFO("Generating meshes for {} nodes...", nodes);

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

        std::unordered_map<uint8_t, std::vector<uint64_t> > maskToChunk;

        for (uint64_t chunk : chunks) {
            uint8_t mask = world.getOctree()->calculateFaceMask(chunk);
            if (mask != 0) {
                maskToChunk[mask].push_back(chunk);
            }
        }
        
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

        m_shader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl"));

        m_shader->bind();
        m_shader->setUniform("lightPos", glm::vec3( -0.2f, -1.0f, -0.3f));

        view.reset(m_shader->locateUniform("view"));
        projection.reset(m_shader->locateUniform("projection"));
        viewPos.reset(m_shader->locateUniform("viewPos"));
        

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

        Boundless::Ref<Boundless::Texture> gPosition;
        gPosition.reset(Boundless::Texture::create2DTexture(800, 600, Boundless::TextureColorChannel::RGBA, Boundless::TextureColorChannel::RGBA16F, NULL));
        gPosition->bind();
        gPosition->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        gPosition->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        gPosition->setTextureParameter(Boundless::TextureParameterName::WRAP_S, Boundless::TextureParameter::CLAMP_TO_EDGE);
        gPosition->setTextureParameter(Boundless::TextureParameterName::WRAP_T, Boundless::TextureParameter::CLAMP_TO_EDGE);
        gPosition->unbind();

        std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaoKernel;
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(
                randomFloats(generator) * 2.0 - 1.0, 
                randomFloats(generator) * 2.0 - 1.0, 
                randomFloats(generator)
            );
            sample  = glm::normalize(sample);
            sample *= randomFloats(generator);
            
            float scale = (float)i / 64.0; 
            scale   = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);  
        }

        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++) {
            glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0, 
                randomFloats(generator) * 2.0 - 1.0, 
                0.0f); 
            ssaoNoise.push_back(noise);
        }  

        Boundless::Ref<Boundless::Texture> noiseTexture;
        noiseTexture.reset(Boundless::Texture::create2DTexture(4, 4, Boundless::TextureColorChannel::RGB, Boundless::TextureColorChannel::RGBA16F, &ssaoNoise[0]));
        noiseTexture->bind();
        noiseTexture->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        noiseTexture->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        noiseTexture->setTextureParameter(Boundless::TextureParameterName::WRAP_S, Boundless::TextureParameter::REPEAT);
        noiseTexture->setTextureParameter(Boundless::TextureParameterName::WRAP_T, Boundless::TextureParameter::REPEAT);
        noiseTexture->unbind();

        Boundless::Ref<Boundless::Texture> ssaoColorBuffer;
        noiseTexture.reset(Boundless::Texture::create2DTexture(800, 600, Boundless::TextureColorChannel::RED, Boundless::TextureColorChannel::RED, &ssaoNoise[0]));
        noiseTexture->bind();
        noiseTexture->setTextureParameter(Boundless::TextureParameterName::MIN_FILTER, Boundless::TextureParameter::NEAREST);
        noiseTexture->setTextureParameter(Boundless::TextureParameterName::MAG_FILTER, Boundless::TextureParameter::NEAREST);
        noiseTexture->unbind();
    }

    void onUpdate() override {
        // glm::mat4 model = glm::mat4(1.0f);
        Boundless::RenderCommand::setClearColor({ 0.1f, 0.3f, 0.1f, 1.0f });
        Boundless::RenderCommand::clear();

        Boundless::Renderer::beginScene();

        m_shader->setUniform(view, m_camera->getViewMatrix());
        m_shader->setUniform(viewPos, m_camera->getPosition());
        m_shader->setUniform(projection, m_camera->getProjectionMatrix());
        
        Boundless::RenderCommand::fillMode();

        for (auto pair : m_toRender) {
            pair.first->bind();
            Boundless::Renderer::submitInstanced(pair.first, pair.second);
        }
        
        Boundless::Renderer::endScene();

    }


};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
