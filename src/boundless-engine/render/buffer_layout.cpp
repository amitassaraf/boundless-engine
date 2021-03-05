#include "buffer_layout.hpp"

namespace Boundless {

    void BufferLayout::calculateOffsetAndStride()  {
                uint32_t offset = 0;
                m_stride = 0;
                for (auto& element : m_elements) {
                    element.offset = offset;
                    offset += element.size;
                    m_stride += element.size;
                }
            }

}