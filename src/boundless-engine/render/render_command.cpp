#include "render_command.hpp"
#include "platform/opengl/opengl_renderer_api.hpp"

namespace Boundless {

    RendererAPI* RenderCommand::s_rendererApi = new OpenGLRendererAPI();

}