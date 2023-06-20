#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "core/buffer.h"
#include "core/color.h"

namespace MR {

class Texture {
    friend class Sampler;

public:
    enum Format {
        UNKNOWN = 0,
        RGB,
        RGB888 = RGB,
        RGB565,
        RGBA,
        RGBA8888 = RGBA,
        RGBA4444,
        RGBA5551,
        ALPHA,
        DEPTH,
    };

    enum Filter {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR,
    };

    enum Wrap { REPEAT, CLAMP };

    enum Type { TEXTURE_2D, TEXTURE_CUBE };

    enum CubeFace { POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z };

    struct Desc {
        uint32_t width;
        uint32_t height;
        uint32_t mip_levels;
        uint32_t array_size;
        Format format;
    };

    class Sampler {
        friend class Texture;

    public:
        virtual ~Sampler();

        static Sampler* create(Texture* texture);

        static Sampler* create(const char* path, bool generate_mipmaps = false);

        void set_wrap_mode(Wrap wrap_s, Wrap wrap_t, Wrap wrap_r = REPEAT);

        void set_filter_mode(Filter minification_filter, Filter magnification_filter);

        Texture* get_texture() const;

        void bind();

    private:
        Sampler(Texture* texture);

        Sampler& operator=(const Sampler&);

        Texture* texture_;
        Wrap wrap_s_;
        Wrap wrap_t_;
        Wrap wrap_r_;
        Filter min_filter_;
        Filter mag_filter_;
    };

public:
    static Texture* create(const char* path, bool generate_mipmaps = false);
    static Texture* create(Format format, unsigned int width, unsigned int height,
                           const unsigned char* data, bool generate_mipmaps = false,
                           Type type = TEXTURE_2D);

    virtual void set_data(const unsigned char* data) = 0;
    virtual void generateMipmaps() = 0;
    virtual const char* get_path() const = 0;
    [[nodiscard]] virtual Type get_type() const = 0;

    inline Format get_format() const { return format_; }
    inline unsigned int get_width() const { return width_; }
    inline unsigned int get_height() const { return height_; }
    inline bool is_mipmapped() const { return mipmapped_; }
    inline bool is_cached() const { return cached_; }
    inline bool is_compressed() const { return compressed_; }
    inline Wrap get_wrap_s() const { return wrap_s_; }
    inline Wrap get_wrap_t() const { return wrap_t_; }
    inline Wrap get_wrap_r() const { return wrap_r_; }
    inline Filter get_min_filter() const { return min_filter_; }
    inline Filter get_mag_filter() const { return mag_filter_; }

    virtual ~Texture();

private:
    Texture();
    Texture(const Texture& copy);
    Texture& operator=(const Texture&);

protected:
    Format format_;
    unsigned int width_;
    unsigned int height_;
    bool mipmapped_;
    bool cached_;
    bool compressed_;
    Wrap wrap_s_;
    Wrap wrap_t_;
    Wrap wrap_r_;
    Filter min_filter_;
    Filter mag_filter_;
};

class Texture2D : public Texture {
public:
    [[nodiscard]] Type get_type() const override { return TEXTURE_2D; }

    virtual void set_pixel(int x, int y, Color color) = 0;
    virtual void set_pixels(std::vector<Color> colors, int mip_level) = 0;
    virtual Color get_pixel(int x, int y) = 0;
    virtual std::vector<Color> get_pixels(int mip_level) = 0;
};

class TextureCube : public Texture {
public:
    [[nodiscard]] Type get_type() const override { return TEXTURE_CUBE; }
};

}  // namespace MR