#pragma once

#include <cstdlib>
#include <fmt/format.h>
#include <ranges>

using namespace std::string_view_literals;

template <typename T>
concept FormatterConcept = requires(T v, const std::string &name, const std::size_t size, const std::uint8_t value) {
    { v.dependencies() } -> std::convertible_to<std::string>;
    { v.start(name, size) } -> std::convertible_to<std::string>;
    { v.step(value) } -> std::convertible_to<std::string>;
    { v.end() } -> std::convertible_to<std::string>;
};

struct formatter {
    std::uint64_t values_per_col{16};

protected:
    std::uint64_t col{};
};

struct std_array_formatter : public formatter {
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

struct std_string_view_formatter : public formatter {
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
