#pragma once
#ifndef BD_OPENGL_BUFFER_HPP_
#define BD_OPENGL_BUFFER_HPP_

#include "render/buffer.hpp"
#include "logging/logger.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace Boundless {

    class OpenGLVertexBuffer : public VertexBuffer {
        public:
            OpenGLVertexBuffer(float* verticies, uint32_t size);
            OpenGLVertexBuffer(uint64_t* verticies, uint32_t size);
            virtual ~OpenGLVertexBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
            virtual uint32_t getCount() const override { return m_size; }

            virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }
            virtual const BufferLayout& getLayout() const override { return m_layout; }

        private:
            uint32_t m_rendererId;
            BufferLayout m_layout;
            uint32_t m_size;
    };

    class OpenGLIndexBuffer : public IndexBuffer {
        public:
            OpenGLIndexBuffer(uint32_t* indicies, uint32_t count);
            virtual ~OpenGLIndexBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
            virtual uint32_t getCount() const override { return m_count; }
        private:
            uint32_t m_rendererId;
            uint32_t m_count;
    };

      class OpenGLFrameBuffer : public FrameBuffer {
        public:
            OpenGLFrameBuffer();
            virtual ~OpenGLFrameBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
            virtual void set2DTexture(unsigned int index, Ref<Texture> texture) const override;
            virtual void setRenderBuffer(FrameBufferAttachmentType type, Ref<RenderBuffer> renderBuffer) const override;
            virtual void enableTextureIndexes(unsigned int indexes[], unsigned int amount) const override;
        protected:
            virtual unsigned int frameBufferAttachmentTypeToRendererValue(FrameBufferAttachmentType type) const override;
        private:
            uint32_t m_rendererId;
    };

    class OpenGLRenderBuffer : public RenderBuffer {
        public:
            OpenGLRenderBuffer(RenderBufferType type, int width, int height);
            virtual ~OpenGLRenderBuffer();

            virtual void bind() const override;
            virtual void unbind() const override;
        protected:
            virtual unsigned int renderBufferTypeToRendererValue(RenderBufferType type) const override;
    };

}

#endif // !BD_OPENGL_BUFFER_HPP_
