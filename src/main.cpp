#include <CLI/CLI.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

#include "formatter.hpp"
#include "raii.hpp"

template <FormatterConcept Formatter>
auto process(const std::filesystem::path &path) -> bool {
    raii::open fd(path.c_str(), O_RDONLY);
    if (!fd) return false;

    std::array<std::uint8_t, 4096> buffer;

    ::posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    std::size_t total = ::lseek64(fd, 0, SEEK_END);
    std::size_t readed{};
    ::lseek64(fd, 0, SEEK_SET);

    Formatter formatter;

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
        process<std_array_formatter>(path);

    return 0;
}
