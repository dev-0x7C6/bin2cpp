#pragma once

#include <cstdlib>
#include <fmt/format.h>
#include <ranges>

using namespace std::string_view_literals;

namespace formatter {

template <typename T>
concept interface = requires(T v, const std::string &name, const std::size_t size, const std::uint8_t value) {
    { v.dependencies() } -> std::convertible_to<std::string>;
    { v.start(name, size) } -> std::convertible_to<std::string>;
    { v.step(value) } -> std::convertible_to<std::string>;
    { v.end() } -> std::convertible_to<std::string>;
};

struct base {
    std::uint64_t values_per_col{16};

protected:
    std::uint64_t col{};
};

} // namespace formatter
