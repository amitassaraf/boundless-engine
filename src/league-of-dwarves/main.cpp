#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {
public:
    std::unordered_map<uint8_t, Boundless::Ref<Boundless::VertexArray> > m_faceMaskToMesh;
    Boundless::Ref<Boundless::Shader> m_shader;
    Boundless::Ref<Boundless::PerspectiveCamera> m_camera;
    std::vector<Boundless::Ref<Boundless::OctreeNode> > chunks;
    std::vector<Boundless::Ref<Boundless::OctreeNode> > airChunks;

    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
        m_camera.reset(new Boundless::PerspectiveCamera(m_eventManager));
        this->pushLayer(m_camera.get());
    }

    ~LeagueOfDwarves() {

    }

    void initialize() override {
        Boundless::World world;
        world.generateWorld();

        world.getOctree()->visitAll(world.getOctree()->getRootNode(), [&](uint32_t nodeLocationalCode, Boundless::Ref<Boundless::OctreeNode>& node) {
            UNUSED(nodeLocationalCode);

            if (!node->isLeaf() || !node->getVoxelData().isSolid() || node->getFaceMask() == 0) {
                airChunks.push_back(node);
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

        Boundless::BufferLayout layout = {
            { Boundless::ShaderDataType::VEC3, "a_Pos" },
            { Boundless::ShaderDataType::VEC3, "surfaceNormal" },
        };
        m_vb->setLayout(layout);

        for (int i = 1; i < 64; i++ ) {
            uint8_t faceMask = i;
            std::vector<uint32_t> cubeIndices;
            m_faceMaskToMesh[faceMask] = Boundless::Ref<Boundless::VertexArray>(Boundless::VertexArray::create());
            m_faceMaskToMesh[faceMask]->addVertexBuffer(m_vb);

            if ((faceMask & FACE_BOTTOM) == FACE_BOTTOM) {
                cubeIndices.insert(cubeIndices.end(), {0, 2, 3,   0, 3, 1});
            }

            if ((faceMask & FACE_TOP) == FACE_TOP) {
                cubeIndices.insert(cubeIndices.end(), {4, 6, 7,   4, 7, 5});
            }

            if ((faceMask & FACE_LEFT) == FACE_LEFT) {
                cubeIndices.insert(cubeIndices.end(), {3, 2, 4,   3, 4, 5});
            }

            if ((faceMask & FACE_RIGHT) == FACE_RIGHT) {
                cubeIndices.insert(cubeIndices.end(), {7, 6, 0,   7, 0, 1});
            }
            
            if ((faceMask & FACE_FRONT) == FACE_FRONT) {
                cubeIndices.insert(cubeIndices.end(), {6, 4, 2,   6, 2, 0});
            }

            if ((faceMask & FACE_BACK) == FACE_BACK) {
                cubeIndices.insert(cubeIndices.end(), {1, 3, 5,   1, 5, 7});
            }

            uint32_t* indicies = &cubeIndices[0];
            Boundless::Ref<Boundless::IndexBuffer> m_ib;
            m_ib.reset(Boundless::IndexBuffer::create(indicies, cubeIndices.size()));
            m_faceMaskToMesh[faceMask]->setIndexBuffer(m_ib);
        }

        std::sort(chunks.begin(), chunks.end(), 
        [](Boundless::Ref<Boundless::OctreeNode> const &a, Boundless::Ref<Boundless::OctreeNode> const &b) {
            return a->getFaceMask() < b->getFaceMask(); 
        });

        // uint32_t cubeIndices[6 * 3 * 2] = {
        //     0, 2, 3,   0, 3, 1, // bottom
        //     4, 6, 7,   4, 7, 5, // top
        //     3, 2, 4,   3, 4, 5, // right
        //     7, 6, 0,   7, 0, 1, // left
        //     6, 4, 2,   6, 2, 0, // back 
        //     1, 3, 5,   1, 5, 7  // front
        // };

        m_shader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl"));
    }

    void onUpdate() override {
        Boundless::RenderCommand::setClearColor({ 0.1f, 0.3f, 0.1f, 1.0f });
        Boundless::RenderCommand::clear();

        Boundless::Renderer::beginScene();

        m_shader->bind();
        
        m_shader->setUniform("lightPos", glm::vec3( -100.0f,  100.0f,  -100.0f));
        m_shader->setUniform("view", m_camera->getViewMatrix());
        m_shader->setUniform("projection", m_camera->getProjectionMatrix());
        
        Boundless::RenderCommand::fillMode();

        uint8_t boundMask = 0;
        for (Boundless::Ref<Boundless::OctreeNode> chunk : chunks) {
            if (boundMask != chunk->getFaceMask()) {
                boundMask = chunk->getFaceMask();
                m_faceMaskToMesh[boundMask]->bind();
            }
            
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 scale = glm::vec3(chunk->getSize(), chunk->getSize(), chunk->getSize());
            m_shader->setUniform("modelScale", glm::scale(model, scale));
            m_shader->setUniform("modelTrans", glm::translate(model, chunk->getChunkOffset()));
            Boundless::Renderer::submit(m_faceMaskToMesh[boundMask]);
        }
        m_faceMaskToMesh[boundMask]->unbind();

        // Boundless::RenderCommand::wireframeMode();

        // for (Boundless::Ref<Boundless::OctreeNode> chunk : airChunks) {
        //     m_faceMaskToMesh[63]->bind();
        //     glm::mat4 model = glm::mat4(1.0f);
        //     glm::vec3 scale = glm::vec3(chunk->getSize(), chunk->getSize(), chunk->getSize());
        //     m_shader->setUniform("modelScale", glm::scale(model, scale));
        //     m_shader->setUniform("modelTrans", glm::translate(model, chunk->getChunkOffset()));
        //     Boundless::Renderer::submit(m_faceMaskToMesh[63]);
        // }
        
        Boundless::Renderer::endScene();

    }


};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
