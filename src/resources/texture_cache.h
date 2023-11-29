#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "shading/texture.h"

namespace MR {

class TextureCache {
public:
    static std::shared_ptr<Texture> load(const std::string& name,
                                         const std::filesystem::path& path,
                                         GLenum target);

private:
    TextureCache();

    static std::unordered_map<unsigned int, std::shared_ptr<Texture>>
        tex_cache_;
};

}  // namespace MR