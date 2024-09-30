#pragma once

#include <cstdint>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <optional>
#include "glad/glad.h"

namespace gl {

const GLuint invalid_id = std::numeric_limits<GLuint>::max();

struct Resolution {
    std::uint32_t width{};
    std::uint32_t height{};
};

enum TexSamples : std::uint32_t {
    MSAA_X1 = 1,
    MSAA_X2 = 2,
    MSAA_X4 = 4,
    MSAA_X8 = 8,
    MSAA_X16 = 16,
    MSAA_X32 = 32,
};

}  // namespace gl

namespace gl::util {

static auto read_file(const std::filesystem::path& path)
    -> std::optional<std::string> {
    if (!std::filesystem::exists(path)) return std::nullopt;

    std::ifstream in_file(path, std::ios::ate | std::ios::binary);

    std::streamsize file_size = in_file.tellg();
    std::string result(file_size, '\0');

    in_file.seekg(0);
    in_file.read(result.data(), file_size);

    return result;
}

static auto get_file_name(std::string_view path) -> std::string {
    std::string_view basename = path.substr(path.find_last_of("/\\") + 1);

    size_t const last_dot = basename.find_last_of(".");
    return std::string(basename.substr(0, last_dot));
}

}  // namespace gl::util