#pragma once

#define BIT(x) (1 << x)
#define UNUSED(x) (void)(x)

template <typename Enumeration>

auto enumToInt(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}