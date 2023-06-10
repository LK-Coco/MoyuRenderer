#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "core/buffer.h"
#include "core/color.h"

namespace MR {

enum WrapMode {
    Wrap_REPEAT,
    Wrap_MIRRORED_REPEAT,
    Wrap_CLAMP_TO_EDGE,
    Wrap_CLAMP_TO_BORDER,
};

enum FilterMode {
    Filter_NEAREST,
    Filter_LINEAR,
    Filter_NEAREST_MIPMAP_NEAREST,
    Filter_LINEAR_MIPMAP_NEAREST,
    Filter_NEAREST_MIPMAP_LINEAR,
    Filter_LINEAR_MIPMAP_LINEAR,
};

enum CubeMapFace {
    TEXTURE_CUBE_MAP_POSITIVE_X = 0,
    TEXTURE_CUBE_MAP_NEGATIVE_X = 1,
    TEXTURE_CUBE_MAP_POSITIVE_Y = 2,
    TEXTURE_CUBE_MAP_NEGATIVE_Y = 3,
    TEXTURE_CUBE_MAP_POSITIVE_Z = 4,
    TEXTURE_CUBE_MAP_NEGATIVE_Z = 5,
};

enum BorderColor {
    Border_BLACK = 0,
    Border_WHITE,
};

struct SamplerDesc {
    FilterMode filterMin = Filter_NEAREST;
    FilterMode filterMag = Filter_NEAREST;

    WrapMode wrapS = Wrap_CLAMP_TO_EDGE;
    WrapMode wrapT = Wrap_CLAMP_TO_EDGE;
    WrapMode wrapR = Wrap_CLAMP_TO_EDGE;

    BorderColor borderColor = Border_BLACK;
};

enum TextureType {
    TextureType_2D,
    TextureType_CUBE,
};

enum TextureFormat {
    TextureFormat_RGBA8 = 0,    // RGBA8888
    TextureFormat_FLOAT32 = 1,  // Float32
};

enum TextureUsage {
    TextureUsage_Sampler = 1 << 0,
    TextureUsage_UploadData = 1 << 1,
    TextureUsage_AttachmentColor = 1 << 2,
    TextureUsage_AttachmentDepth = 1 << 3,
    TextureUsage_RendererOutput = 1 << 4,
};

struct TextureDesc {
    int width = 0;
    int height = 0;
    TextureType type = TextureType_2D;
    TextureFormat format = TextureFormat_RGBA8;
    uint32_t usage = TextureUsage_Sampler;
    bool useMipmaps = false;
    bool multiSample = false;
    std::string tag;
};

class Texture : public TextureDesc {
public:
    virtual ~Texture() = default;

    inline uint32_t get_level_width(uint32_t level) { return std::max(1, width >> level); }

    inline uint32_t get_level_height(uint32_t level) { return std::max(1, height >> level); }

    virtual int get_id() const = 0;
    virtual void set_sampler_desc(SamplerDesc &sampler){};
    virtual void init_image_data(){};
    virtual void set_image_data(const std::vector<Buffer<Color> *> &buffers){};
    virtual void dump_image(const char *path, uint32_t layer, uint32_t level) = 0;
};

}  // namespace MR