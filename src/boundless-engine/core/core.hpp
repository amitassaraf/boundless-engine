#pragma once

#include <memory>

#define BIT(x) (1 << x)
#define UNUSED(x) (void)(x)

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