#pragma once

#include <memory>

#define BIT(x) (1 << x)
#define UNUSED(x) (void)(x)

#define TRANSFORM_16_BIT_NEAREST_POWER_OF_TWO(x) x--; \
                                x |= x >> 1; \
                                x |= x >> 2; \
                                x |= x >> 4; \
                                x |= x >> 8; \
                                x++;

#define TRANSFORM_32_BIT_NEAREST_POWER_OF_TWO(x) x--; \
                                x |= x >> 1; \
                                x |= x >> 2; \
                                x |= x >> 4; \
                                x |= x >> 8; \
                                x |= x >> 16; \
                                x++;

template <typename Enumeration>

auto enumToInt(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

namespace Boundless {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;

}
