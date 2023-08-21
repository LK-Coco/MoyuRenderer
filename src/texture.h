#pragma once

#include <string>

namespace MR {

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;

    Texture(const char* file_path);
};
}  // namespace MR