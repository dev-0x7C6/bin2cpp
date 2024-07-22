#include <CLI/CLI.hpp>
#include <fmt/format.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <ranges>
#include <string>

#include "raii.hpp"

using namespace std::string_view_literals;

struct formatter {
    std::uint64_t values_per_col{16};

protected:
    std::uint64_t col{};
};

struct std_array_formatter : public formatter {
    constexpr auto dependencies() {
        return fmt::format("#include <array>\n\n");
    }

    constexpr auto start(const std::string &name, const std::size_t size) {
        return fmt::format("static constexpr std::array<unsigned char, {}> {}_array = {}", size,
            name | std::ranges::views::transform([](char v) {
                return v == '.' ? '_' : v;
            }) | std::ranges::to<std::string>(),
            '{');
    }

    constexpr auto step(const std::uint8_t value) {
        return fmt::format("{}0x{:02x}, ", (col++ % values_per_col == 0) ? "\n  " : "", value);
    }

    constexpr auto end() {
        return fmt::format("\n{};\n", '}');
    }
};

struct std_string_view_formatter : public formatter {
    constexpr auto dependencies() {
        return fmt::format("#include <string_view>\n\n");
    }

    constexpr auto start(const std::string &name, const std::size_t) {
        return fmt::format("static constexpr std::string_view {}_sv =\n",
            name | std::ranges::views::transform([](char v) {
                return v == '.' ? '_' : v;
            }) | std::ranges::to<std::string>());
    }

    constexpr auto step(const std::uint8_t value) {
        auto ret = fmt::format("{}\\x{:02x}{}",
            (col % values_per_col == 0) ? "  \"" : "",
            value,
            (col % values_per_col == values_per_col - 1) ? "\"\n" : "");
        col++;
        return ret;
    }

    constexpr auto end() {
        return fmt::format("{};\n\n", (col++ % values_per_col == 0) ? "" : "\"");
    }

private:
    std::uint64_t col{};
};

auto process(const std::filesystem::path &path) -> bool {
    raii::open fd(path.c_str(), O_RDONLY);
    if (!fd) return false;

    std::array<std::uint8_t, 4096> buffer;

    ::posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    std::size_t total = ::lseek64(fd, 0, SEEK_END);
    std::size_t readed{};
    ::lseek64(fd, 0, SEEK_SET);

    std_array_formatter formatter;

    std::string output;
    output.reserve(total * 8 + 4096);

    output += formatter.dependencies();
    output += formatter.start(path.filename(), total);

    for (;;) {
        const auto size = ::read(fd, buffer.data(), buffer.size());
        if (size == 0) break;
        if (size <= 0) return false;

        for (auto i = 0; i < size; ++i)
            output += formatter.step(buffer[i]);

        readed += size;
    }

    output += formatter.end();

    fmt::print("{}", output);
    return total == readed;
}

auto main(int argc, char **argv) -> int {
    CLI::App app("bin2cpp");

    std::vector<std::string> paths;

    app.add_option("-i,--input", paths, "file / files") //
        ->allow_extra_args()
        ->check(CLI::ExistingFile);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return 1;
    }

    for (auto &&path : paths)
        process(path);

    return 0;
}
