#pragma once

#include "interface.hpp"

namespace formatter {

struct to_std_array : public base {
    constexpr auto dependencies() -> std::string {
        return fmt::format("#include <array>\n\n");
    }

    constexpr auto start(const std::string &name, const std::size_t size) -> std::string {
        return fmt::format("static constexpr std::array<unsigned char, {}> {}_array = {}", size,
            name | std::ranges::views::transform([](char v) {
                return v == '.' ? '_' : v;
            }) | std::ranges::to<std::string>(),
            '{');
    }

    constexpr auto step(const std::uint8_t value) -> std::string {
        return fmt::format("{}0x{:02x}, ", (col++ % values_per_col == 0) ? "\n  " : "", value);
    }

    constexpr auto end() -> std::string {
        return fmt::format("\n{};\n", '}');
    }
};

} // namespace formatter
