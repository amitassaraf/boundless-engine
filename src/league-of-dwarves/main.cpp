#include <boundless.h>

class LeagueOfDwarves : public Boundless::Game {
public:
    Boundless::Ref<Boundless::VertexArray> m_va;
    Boundless::Ref<Boundless::Shader> m_shader;

    LeagueOfDwarves() {
        BD_GAME_INFO("Starting league of dwarves.");
        this->pushLayer(new Boundless::WindowLayer(m_eventManager));
    }

    ~LeagueOfDwarves() {

    }

    void initialize() override {
        // Boundless::World world;
        // world.generateWorld();

        m_va.reset(Boundless::VertexArray::create());

        float cubeVertices [] = {
            -1.0, -1.0,  1.0,
            1.0, -1.0,  1.0,
            -1.0,  1.0,  1.0,
            1.0,  1.0,  1.0,
            -1.0, -1.0, -1.0,
            1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
            1.0,  1.0, -1.0,
        };

        uint32_t cubeIndices [] = {
            0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
        };

        // float verticies [3 * 4] = {
        //     -0.5f, -0.5f, 0.0f,
        //     0.5f, -0.5f, 0.0f,
        //     0.5f, 0.5f, 0.0f,
        //     -0.5f, 0.5f, 0.0f
        // };

        Boundless::Ref<Boundless::VertexBuffer> m_vb;
        m_vb.reset(Boundless::VertexBuffer::create(cubeVertices, sizeof(cubeVertices)));

        Boundless::BufferLayout layout = {
            { Boundless::ShaderDataType::VEC3, "a_Pos" },
        };
        m_vb->setLayout(layout);
        m_va->addVertexBuffer(m_vb);

        // uint32_t indicies[6] = {
        //     0, 1, 2, 2, 3, 0
        // };

        Boundless::Ref<Boundless::IndexBuffer> m_ib;
        m_ib.reset(Boundless::IndexBuffer::create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));
        m_va->setIndexBuffer(m_ib);

        m_shader.reset(Boundless::Shader::create("/Users/amitassaraf/workspace/league_of_dwarves/assets/shaders/opengl"));
    }

    void onUpdate() override {
        Boundless::RenderCommand::setClearColor({ 0.1f, 0.3f, 0.1f, 1.0f });
        Boundless::RenderCommand::clear();
        Boundless::RenderCommand::wireframeMode();
        
        Boundless::Renderer::beginScene();

        m_shader->bind();
        Boundless::Renderer::submit(m_va);
        
        Boundless::Renderer::endScene();

    }


};

namespace Boundless {
    Game* create_boundless_game() {
    return new LeagueOfDwarves();
    }
}
