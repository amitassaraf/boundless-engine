#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {
public:
    Boundless::Ref<Boundless::VertexArray> m_va;
    Boundless::Ref<Boundless::Shader> m_shader;
    Boundless::Ref<Boundless::PerspectiveCamera> m_camera;

    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
        m_camera.reset(new Boundless::PerspectiveCamera(m_eventManager));
        this->pushLayer(m_camera.get());
    }

    ~LeagueOfDwarves() {

    }

    void initialize() override {
        // Boundless::World world;
        // world.generateWorld();

        m_va.reset(Boundless::VertexArray::create());

        float cubeVertices[3 * 8 * 2] = {
            -1, -1, -1,   0, -1,  0,  // 0, nv front
            -1, -1,  1,   0,  0,  1,  // 1, nv top
            1, -1, -1,   0,  0,  0,  // 2
            1, -1,  1,   1,  0,  0,  // 3, nv right
            1,  1, -1,   0,  1,  0,  // 4, nv back
            1,  1,  1,   0,  0,  0,  // 5
            -1,  1, -1,   0,  0, -1,  // 6, nv bottom
            -1,  1,  1,  -1,  0,  0,  // 7, nv left 
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
        // Boundless::RenderCommand::wireframeMode();

        Boundless::Renderer::beginScene();

        m_shader->bind();
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3( 0.0f,  0.0f,  0.0f));

        m_shader->setUniform("model", model);
        m_shader->setUniform("lightPos", glm::vec3( 5.0f,  5.0f,  5.0f));
        m_shader->setUniform("view", m_camera->getViewMatrix());
        m_shader->setUniform("projection", m_camera->getProjectionMatrix());
        Boundless::Renderer::submit(m_va);
        
        Boundless::Renderer::endScene();

    }


};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
