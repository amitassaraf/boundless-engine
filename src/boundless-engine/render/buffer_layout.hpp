#pragma once
#ifndef BD_BUFFER_LAYOUT_HPP_
#define BD_BUFFER_LAYOUT_HPP_

#include "shader.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Boundless {

    struct BufferElement {
        std::string name;
        ShaderDataType type;
        uint32_t size;
        uint32_t offset;
        bool normalized;

        BufferElement() {}
        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) :
            name(name), type(type), size(getShaderDataTypeSize(type)),  offset(0), normalized(normalized) {

            }
    };

    class BufferLayout {
        public:
            BufferLayout() {}
            BufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements) {
                calculateOffsetAndStride();
            }

            inline uint32_t getStride() const { return m_stride; }
            inline const std::vector<BufferElement>& getElements() const { return m_elements; }
            std::vector< BufferElement >::iterator begin() { return m_elements.begin(); }
            std::vector< BufferElement >::iterator end() { return m_elements.end(); }
            std::vector< BufferElement >::const_iterator begin() const { return m_elements.begin(); }
            std::vector< BufferElement >::const_iterator end() const { return m_elements.end(); }
        private:
            void calculateOffsetAndStride();

            std::vector<BufferElement> m_elements;
            uint32_t m_stride = 0;
    };

}

#endif // !BD_BUFFER_LAYOUT_HPP_
