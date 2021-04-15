#pragma once
#ifndef BD_BUFFER_HPP_
#define BD_BUFFER_HPP_

#include <cstdint>
#include "buffer_layout.hpp"
#include "texture.hpp"

namespace Boundless {

    class VertexBuffer {
        public:
            virtual ~VertexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            virtual void setLayout(const BufferLayout& layout) = 0;
            virtual const BufferLayout& getLayout() const = 0;
            virtual uint32_t getCount() const = 0;

            static VertexBuffer* create(float* verticies, uint32_t size);
    };

    class IndexBuffer {
        public:
            virtual ~IndexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            virtual uint32_t getCount() const = 0;

            static IndexBuffer* create(uint32_t* indicies, uint32_t count);
    };

    enum class RenderBufferType {
        DEPTH_BUFFER = 0,
    };

    class RenderBuffer {
        public:
            virtual ~RenderBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            inline uint32_t getRendererID() const { return m_rendererId;  }

            static RenderBuffer* create(RenderBufferType type, int width, int height);
        protected:
            virtual unsigned int renderBufferTypeToRendererValue(RenderBufferType type) const = 0;
            uint32_t m_rendererId;
    };

    enum class FrameBufferAttachmentType {
        DEPTH = 0,
    };

    class FrameBuffer {
        public:
            virtual ~FrameBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
            virtual void set2DTexture(unsigned int index, Ref<Texture> texture) const = 0;
            virtual void enableTextureIndexes(unsigned int indexes[], unsigned int amount) const = 0;
            virtual void setRenderBuffer(FrameBufferAttachmentType type, Ref<RenderBuffer> renderBuffer) const = 0;

            static FrameBuffer* create();
        protected:
            virtual unsigned int frameBufferAttachmentTypeToRendererValue(FrameBufferAttachmentType type) const = 0;
    };

}

#endif // !BD_BUFFER_HPP_
