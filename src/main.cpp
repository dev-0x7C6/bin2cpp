#include <CLI/CLI.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>

#include "formatter/interface.hpp"
#include "formatter/to_std_array.hpp"
#include "formatter/to_std_string_view.hpp"

#include "raii.hpp"

template <formatter::interface formatter>
auto process(const std::filesystem::path &path) -> bool {
    raii::open fd(path.c_str(), O_RDONLY);
    if (!fd) return false;

    std::array<std::uint8_t, 4096> buffer;

    ::posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    std::size_t total = ::lseek64(fd, 0, SEEK_END);
    std::size_t readed{};
    ::lseek64(fd, 0, SEEK_SET);

    formatter format;

    std::string output;
    output.reserve(total * 8 + 4096);

    output += format.dependencies();
    output += format.start(path.filename(), total);

    for (;;) {
        const auto size = ::read(fd, buffer.data(), buffer.size());
        if (size == 0) break;
        if (size <= 0) return false;

        for (auto i = 0; i < size; ++i)
            output += format.step(buffer[i]);

        readed += size;
    }

    output += format.end();

    fmt::print("{}", output);
    return total == readed;
}

enum class formatter_type {
    as_std_array,
    as_std_string_view,
};

auto generate(const formatter_type type, const std::filesystem::path &path) {
    switch (type) {
        case formatter_type::as_std_array:
            return process<formatter::to_std_array>(path);
        case formatter_type::as_std_string_view:
            return process<formatter::to_std_string_view>(path);
    }

    return false;
}

auto main(int argc, char **argv) -> int {
    CLI::App app("bin2cpp");

    std::vector<std::string> paths;
    auto format_type = formatter_type::as_std_array;

    app.add_option("-i,--input", paths, "file / files") //
        ->allow_extra_args()
        ->check(CLI::ExistingFile);

    app.add_option("--sa,--std-array", [&](auto &&) {
        format_type = formatter_type::as_std_array;
        return true; }, "convert to std::array")->expected(0);

    app.add_option("--sv,--string-view", [&](auto &&) {
        format_type = formatter_type::as_std_string_view;
        return true; }, "convert to std::string_view")->expected(0);

    try {
        app.parse(argc, argv);
    } catch (const CLI::CallForHelp &) {
        std::cerr << app.help() << std::endl;
        return 1;
    } catch (const CLI::ParseError &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    for (auto &&path : paths)
        generate(format_type, path);

    return 0;
}
