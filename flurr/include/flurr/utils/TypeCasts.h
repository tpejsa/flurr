#pragma once

#include <type_traits>

namespace flurr
{

template <typename E>
constexpr auto FromEnum(E a_value) -> typename std::underlying_type<E>::type
{
  return static_cast<typename std::underlying_type<E>::type>(a_value);
}

template <typename E>
constexpr E ToEnum(typename std::underlying_type<E>::type a_value)
{
  return static_cast<E>(a_value);
}

} // namespace flurr
