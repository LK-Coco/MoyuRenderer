#pragma once

#include <string>

namespace MR {

class SceneMeta {
public:
    SceneMeta(const std::string& file_path);

public:
    std::string material_type;
};

}  // namespace MR