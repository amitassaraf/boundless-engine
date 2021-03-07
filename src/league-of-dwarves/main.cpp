#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {
public:
    Boundless::Ref<Boundless::VertexArray> m_va;
    Boundless::Ref<Boundless::Shader> m_shader;
    Boundless::Ref<Boundless::PerspectiveCamera> m_camera;
    std::vector<std::pair<glm::vec3, uint32_t> > chunks;
    std::vector<std::pair<glm::vec3, uint32_t> > airChunks;

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

        world.m_octree->visitAll(world.m_octree->getRootNode(), [&](uint32_t nodeLocationalCode, Boundless::OctreeNode* node) {
            UNUSED(nodeLocationalCode);
            glm::vec3 origin = node->getChunkOffset();

            if (!node->isLeaf() || !node->m_solid) {
                airChunks.push_back(std::make_pair(origin, node->getSize()));
                return;
            }

            chunks.push_back(std::make_pair(origin, node->getSize()));
        });
        // BD_CORE_TRACE("SIZE: {}", world.m_octree->getRootNode()->getSize());
        // chunks.push_back(std::make_pair(world.m_octree->getRootNode()->getChunkOffset(), world.m_octree->getRootNode()->getSize()));
        BD_CORE_TRACE("LENGTH: {}", chunks.size());

        m_va.reset(Boundless::VertexArray::create());

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

        uint32_t cubeIndices[6 * 3 * 2] = {
            0, 2, 3,   0, 3, 1, // front
            4, 6, 7,   4, 7, 5, // back
            3, 2, 4,   3, 4, 5, // right
            7, 6, 0,   7, 0, 1, // left
            6, 4, 2,   6, 2, 0, // bottom 
            1, 3, 5,   1, 5, 7  // top
        };

        Boundless::Ref<Boundless::VertexBuffer> m_vb;
        m_vb.reset(Boundless::VertexBuffer::create(cubeVertices, sizeof(cubeVertices)));

        Boundless::BufferLayout layout = {
            { Boundless::ShaderDataType::VEC3, "a_Pos" },
            { Boundless::ShaderDataType::VEC3, "surfaceNormal" },
        };
        m_vb->setLayout(layout);
        m_va->addVertexBuffer(m_vb);

        Boundless::Ref<Boundless::IndexBuffer> m_ib;
        m_ib.reset(Boundless::IndexBuffer::create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));
        m_va->setIndexBuffer(m_ib);

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

        m_va->bind();
        for (std::pair<glm::vec3, uint32_t> chunkPair : chunks) {
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 scale = glm::vec3(chunkPair.second, chunkPair.second, chunkPair.second);
            m_shader->setUniform("modelScale", glm::scale(model, scale));
            m_shader->setUniform("modelTrans", glm::translate(model, chunkPair.first));
            Boundless::Renderer::submit(m_va);
        }

        // Boundless::RenderCommand::wireframeMode();

        // for (std::pair<glm::vec3, uint32_t> chunkPair : airChunks) {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     glm::vec3 scale = glm::vec3(chunkPair.second, chunkPair.second, chunkPair.second);
        //     m_shader->setUniform("modelScale", glm::scale(model, scale));
        //     m_shader->setUniform("modelTrans", glm::translate(model, chunkPair.first));
        //     Boundless::Renderer::submit(m_va);
        // }
        
        Boundless::Renderer::endScene();

    }


};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
