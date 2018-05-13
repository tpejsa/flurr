#pragma once

#include <type_traits>

namespace flurr {

template <typename E>
constexpr auto FromEnum(E value) -> typename std::underlying_type<E>::type {
  return static_cast<typename std::underlying_type<E>::type>(value);
}

template <typename E>
constexpr E ToEnum(typename std::underlying_type<E>::type value) {
  return static_cast<E>(value);
}

} // namespace flurr
