#include "texture_cache.h"
#include <memory>
#include "glm/common.hpp"
#include "utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace MR {

std::unordered_map<unsigned int, std::shared_ptr<Texture>>
    TextureCache::tex_cache_{};

std::shared_ptr<Texture> TextureCache::load(const std::string& name,
                                            const std::filesystem::path& path,
                                            GLenum target) {
    auto id = SHASH(name);

    if (auto item = tex_cache_.find(id); item != tex_cache_.end()) {
        return item->second;
    }

    // TODO check path valid

    auto p = std::filesystem::absolute(path);

    stbi_set_flip_vertically_on_load(true);

    auto f = stbi__fopen(p.string().c_str(), "rb");
    assert(f);
    const auto hdr = stbi_is_hdr_from_file(f);

    int width, height, nr_components;
    auto data =
        hdr ? (void*)stbi_loadf_from_file(f, &width, &height, &nr_components, 0)
            : (void*)stbi_load_from_file(f, &width, &height, &nr_components, 0);
    fclose(f);
    assert(data);

    std::shared_ptr<Texture> tex = std::make_shared<Texture>(target);
    tex->is_hdr = hdr;
    uint32_t num_levels{1u};
    if (Utils::is_power_of2(width) && Utils::is_power_of2(height))
        num_levels = Utils::calc_mip_levels(glm::max(width, height));
    GLenum internal_format;
    GLenum format;
    if (nr_components == 1) {
        format = GL_RED;
        internal_format = GL_R8;
    } else if (nr_components == 3) {
        format = GL_RGB;
        internal_format = hdr ? GL_RGB16F : GL_RGB8;
    } else if (nr_components == 4) {
        format = GL_RGBA;
        internal_format = hdr ? GL_RGBA16F : GL_RGBA8;
    }

    if (target == GL_TEXTURE_1D) {
        tex->set_storage_1d(num_levels, internal_format, width);
        tex->set_sub_image_1d(num_levels, 0, width, format, GL_UNSIGNED_BYTE,
                              data);
    } else if (target == GL_TEXTURE_2D) {
        tex->set_storage_2d(num_levels, internal_format, width, height);
        tex->set_sub_image_2d(num_levels, 0, 0, width, height, format,
                              GL_UNSIGNED_BYTE, data);
    } else if (target == GL_TEXTURE_CUBE_MAP) {
        tex->set_storage_2d(num_levels, internal_format, width, height);
        tex->set_sub_image_3d(num_levels, 0, 0, 6, width, height, 1, format,
                              GL_FLOAT, data);
    }
    const auto is_mipmap = num_levels > 1;
    tex->set(GL_TEXTURE_MIN_FILTER,
             is_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    tex->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (is_mipmap) tex->generate_mipmap();

    stbi_image_free(data);

    tex_cache_[id] = tex;

    return tex_cache_[id];
}

}  // namespace MR