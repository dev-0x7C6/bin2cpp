#pragma once

#include "interface.hpp"

namespace formatter {

struct to_std_string_view : public base {
    constexpr auto dependencies() -> std::string {
        return fmt::format("#include <string_view>\n\n");
    }

    constexpr auto start(const std::string &name, const std::size_t) -> std::string {
        return fmt::format("static constexpr std::string_view {}_sv =\n",
            name | std::ranges::views::transform([](char v) {
                return v == '.' ? '_' : v;
            }) | std::ranges::to<std::string>());
    }

    constexpr auto step(const std::uint8_t value) -> std::string {
        auto ret = fmt::format("{}\\x{:02x}{}",
            (col % values_per_col == 0) ? "  \"" : "",
            value,
            (col % values_per_col == values_per_col - 1) ? "\"\n" : "");
        col++;
        return ret;
    }

    constexpr auto end() -> std::string {
        return fmt::format("{};\n\n", (col++ % values_per_col == 0) ? "" : "\"");
    }

private:
    std::uint64_t col{};
};

} // namespace formatter
