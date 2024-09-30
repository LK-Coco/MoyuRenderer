#pragma once

#include <array>
#include "utility.h"

namespace gl {

template <GLenum target>
class Texture {
public:
    Texture() { glCreateTextures(target, 1, &id_); }

    explicit Texture(const GLuint id) : id_(id), managed_(false) {}

    Texture(const Texture& that) = delete;
    Texture(Texture&& temp) noexcept : id_(temp.id_), managed_(temp.managed_) {
        temp.id_ = invalid_id;
        temp.managed_ = false;
    }

    Texture& operator=(const Texture& that) = delete;
    Texture& operator=(Texture&& temp) noexcept {
        if (this != &temp) {
            if (managed_ && id_ != invalid_id) glDeleteTextures(1, &id_);

            id_ = temp.id_;
            managed_ = temp.managed_;

            temp.id_ = invalid_id;
            temp.managed_ = false;
        }
        return *this;
    }

    virtual ~Texture() {
        if (managed_ && id_ != invalid_id) glDeleteTextures(1, &id_);
    }

    void bind() const { glBindTexture(target, id_); }

    void bind_unit(const GLuint unit) const { glBindTextureUnit(unit, id_); }

    [[nodiscard]] bool is_valid() const { return glIsTexture(id_); }

    void generate_mipmap() const { glGenerateTextureMipmap(id_); }

    void set_data(GLenum internal_format, const GLsizei width,
                  const GLsizei height, const GLenum format, const GLenum type,
                  const void* data) {
        set_storage(1, internal_format, width, height);
        set_sub_image(0, 0, 0, width, height, format, type, data);
    }

    void set_storage(const GLsizei levels, const GLenum internal_format,
                     const GLsizei width, const GLsizei height) const {
        glTextureStorage2D(id_, levels, internal_format, width, height);
    }

    void set_sub_image(const GLint level, const GLint x, const GLint y,
                       const GLsizei width, const GLsizei height,
                       const GLenum format, const GLenum type,
                       const void* data) const {
        glTextureSubImage2D(id_, level, x, y, width, height, format, type,
                            data);
    }

    void set_wrap_st(const GLenum s_mod, const GLenum t_mod) const {
        glTextureParameteri(id_, GL_TEXTURE_WRAP_S, s_mod);
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T, t_mod);
    }

    void set_wrap_s(const GLenum mode) const {
        glTextureParameteri(id_, GL_TEXTURE_WRAP_S, mode);
    }
    void set_wrap_t(const GLenum mode) const {
        glTextureParameteri(id_, GL_TEXTURE_WRAP_T, mode);
    }
    void set_wrap_r(const GLenum mode) const {
        glTextureParameteri(id_, GL_TEXTURE_WRAP_R, mode);
    }

    void set_border_color(const std::array<GLfloat, 4>& color) const {
        glTextureParameterfv(id_, GL_TEXTURE_BORDER_COLOR, color.data());
    }

    void set_filter(const GLenum min_filter, const GLenum magh_filter) const {
        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, min_filter);
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, magh_filter);
    }

    void set_min_filter(const GLenum mode) const {
        glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, mode);
    }
    void set_mag_filter(const GLenum mode) const {
        glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, mode);
    }

    void set_compare(const GLenum mode, const GLenum function) const {
        glTextureParameteri(id_, GL_TEXTURE_COMPARE_MODE, mode);
        glTextureParameteri(id_, GL_TEXTURE_COMPARE_FUNC, function);
    }

    void set_compare_mode(const GLenum mode) const {
        glTextureParameteri(id_, GL_TEXTURE_COMPARE_MODE, mode);
    }
    void set_compare_func(const GLenum function) const {
        glTextureParameteri(id_, GL_TEXTURE_COMPARE_FUNC, function);
    }

    [[nodiscard]] GLenum wrap_s() const {
        return get_int_parameter(GL_TEXTURE_WRAP_S);
    }
    [[nodiscard]] GLenum wrap_t() const {
        return get_int_parameter(GL_TEXTURE_WRAP_T);
    }
    [[nodiscard]] GLenum wrap_r() const {
        return get_int_parameter(GL_TEXTURE_WRAP_R);
    }

    [[nodiscard]] GLenum min_filter() const {
        return get_int_parameter(GL_TEXTURE_MIN_FILTER);
    }
    [[nodiscard]] GLenum mag_filter() const {
        return get_int_parameter(GL_TEXTURE_MAG_FILTER);
    }

    [[nodiscard]] GLsizei width(const GLuint level = 0) const {
        return get_int_level_parameter(level, GL_TEXTURE_WIDTH);
    }
    [[nodiscard]] GLsizei height(const GLuint level = 0) const {
        return get_int_level_parameter(level, GL_TEXTURE_HEIGHT);
    }
    [[nodiscard]] GLsizei depth(const GLuint level = 0) const {
        return get_int_level_parameter(level, GL_TEXTURE_DEPTH);
    }

    [[nodiscard]] GLenum internal_format(const GLuint level = 0) const {
        return get_int_level_parameter(level, GL_TEXTURE_INTERNAL_FORMAT);
    }

    [[nodiscard]] GLuint id() const { return id_; }

public:
    static void unbind() { glBindTexture(target, 0); }

    static void set_active(const GLenum unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
    }

protected:
    template <std::size_t count>
    std::array<GLint, count> get_int_parameter(const GLenum parameter) const {
        std::array<GLint, count> result;
        glGetTextureParameteriv(id_, parameter, result.data());
        return result;
    }
    [[nodiscard]] GLint get_int_parameter(const GLenum parameter) const {
        GLint result;
        glGetTextureParameteriv(id_, parameter, &result);
        return result;
    }

    template <std::size_t count>
    std::array<GLint, count> get_int_level_parameter(
        const GLuint level, const GLenum parameter) const {
        std::array<GLint, count> result;
        glGetTextureLevelParameteriv(id_, level, parameter, result.data());
        return result;
    }
    [[nodiscard]] GLint get_int_level_parameter(const GLuint level,
                                                const GLenum parameter) const {
        GLint result;
        glGetTextureLevelParameteriv(id_, level, parameter, &result);
        return result;
    }

private:
    GLuint id_ = invalid_id;
    bool managed_ = true;
};

using Texture2D = Texture<GL_TEXTURE_2D>;
using TextureCube = Texture<GL_TEXTURE_CUBE_MAP>;

}  // namespace gl