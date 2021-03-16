#include <boundless.h>
#include <glm/gtx/string_cast.hpp>
#include <algorithm> 

class LeagueOfDwarves : public Boundless::Game {
public:
    std::vector<std::pair<Boundless::Ref<Boundless::VertexArray>, uint32_t> > m_toRender;
    Boundless::Ref<Boundless::Shader> m_shader;
    Boundless::Ref<Boundless::PerspectiveCamera> m_camera;
    Boundless::Ref<Boundless::LocatedUniform> view;
    Boundless::Ref<Boundless::LocatedUniform> projection;
    Boundless::World world;

    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
        m_camera.reset(new Boundless::PerspectiveCamera(m_eventManager));
        this->pushLayer(m_camera.get());
        this->pushLayer(new Boundless::FPSCounterLayer(m_eventManager));
    }

    ~LeagueOfDwarves() {

    }

    void calcRenderNodes(Boundless::World& world) {
        std::vector<std::reference_wrapper<Boundless::Ref<Boundless::OctreeNode> > > chunks;
        m_toRender.clear();
        world.getOctree()->visitAll(world.getOctree()->getRootNode(), [&](uint64_t nodeLocationalCode, Boundless::Ref<Boundless::OctreeNode>& node) {
            UNUSED(nodeLocationalCode);

            if (!node->isLeaf() || !node->getVoxelData().isSolid() || node->getFaceMask() == 0) {
                return;
            }

            chunks.push_back(node);
        });
        
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

        BD_CORE_INFO("Generating meshes...");
        
        for (int i = 1; i < 64; i++ ) {
            uint8_t faceMask = i;

            std::vector<float> cubePositions;
            uint32_t instanceCount = 0u;
            for (std::reference_wrapper<Boundless::Ref<Boundless::OctreeNode> > chunk : chunks) {
                if (chunk.get()->getFaceMask() == faceMask) {
                    glm::vec3 offset = chunk.get()->getChunkOffset();
                    cubePositions.push_back(offset.x);
                    cubePositions.push_back(offset.y);
                    cubePositions.push_back(offset.z);
                    cubePositions.push_back((float)chunk.get()->getSize());
                    
                    instanceCount += 1u;
                }
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

    void initialize() override {
        world.generateWorld();

        calcRenderNodes(world);

        m_shader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl"));

        m_shader->bind();
        m_shader->setUniform("lightPos", glm::vec3( 0,  256,  0));

        view.reset(m_shader->locateUniform("view"));
        projection.reset(m_shader->locateUniform("projection"));

        m_eventManager.appendListener(Boundless::EventType::KEY_PRESSED, [&](const Boundless::Ref<Boundless::Event> event) {
            Boundless::Ref<Boundless::KeyPressedEvent> keyPressedEvent = std::dynamic_pointer_cast<Boundless::KeyPressedEvent> (event);
            
            if (keyPressedEvent->getKeyCode() == 84) {
                checkLODChanges();
            }
        });
    }

    void checkLODChanges() {
        Boundless::Scope<Boundless::Octree>& octree = world.getOctree();
        std::vector<uint64_t > chunkChanges;
        glm::vec3 camera = m_camera->getPosition();

        octree->visitAllConditional(octree->getRootNode(), [&](uint64_t nodeLocationalCode, Boundless::Ref<Boundless::OctreeNode>& node) {
            UNUSED(nodeLocationalCode);
            if (node) {
                glm::vec3 chunkLocation = node->getChunkOffset();
                glm::vec3 chunkCenter(chunkLocation.x + (node->getSize() / 2), chunkLocation.y + (node->getSize() / 2), chunkLocation.z + (node->getSize() / 2));
                auto distance = abs(glm::length(camera - chunkCenter));

                if (distance < (node->getSize() * 50)) {
                    if (node->getChildrenMask() == 0) {
                        world.changeLOD(node, DIVIDE);
                    }
                    return true;
                }
                if (node->getChildrenMask() != 0) {
                    chunkChanges.push_back(nodeLocationalCode);
                }
            }
            return false;
        });

        for (uint64_t location : chunkChanges) {
            // Do stuff
            if (octree->nodeExists(location)) {
                world.changeLOD(octree->getNodeAt(location), COLLAPSE);
            }
        }
        
        calcRenderNodes(world);
    }

    void onUpdate() override {
        // glm::mat4 model = glm::mat4(1.0f);
        Boundless::RenderCommand::setClearColor({ 0.1f, 0.3f, 0.1f, 1.0f });
        Boundless::RenderCommand::clear();

        Boundless::Renderer::beginScene();

        m_shader->setUniform(view, m_camera->getViewMatrix());
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
