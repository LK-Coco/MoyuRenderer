#pragma once

#include <iostream>
#include <unordered_map>
#include <map>
#include <any>
#include "glad/glad.h"
#include "utility.h"
#include "glm/glm.hpp"

namespace gl {

class Shader {
public:
    Shader(const std::filesystem::path& vertex_path,
           const std::filesystem::path& fragment_path,
           const std::map<std::string, std::any>& defines);
    Shader(const std::filesystem::path& vertex_path,
           const std::filesystem::path& fragment_path,
           const std::filesystem::path& geometry_path,
           const std::map<std::string, std::any>& defines);
    Shader(const std::filesystem::path& comp_path,
           const std::map<std::string, std::any>& defines);

    ~Shader() {
        if (id_ != 0) glDeleteProgram(id_);
    }

    Shader(const Shader&) = delete;
    auto operator=(const Shader&) -> Shader& = delete;

    Shader(Shader&& other) noexcept
        : uniform_cache_{std::move(other.uniform_cache_)},
          id_{other.id_} {
        other.id_ = 0;
    }

    auto operator=(Shader&& other) noexcept -> Shader& {
        if (this != &other) {
            uniform_cache_ = std::move(other.uniform_cache_);
            id_ = other.id_;
            other.id_ = 0;
        }
        return *this;
    }

    [[nodiscard]] constexpr auto get_id() const -> std::uint32_t { return id_; }

    void set_bool(std::string_view name, bool val);
    void set_int(std::string_view name, int val);
    void set_float(std::string_view name, float val);
    void set_vec2(std::string_view name, const glm::vec2& val);
    void set_vec2(std::string_view name, float x, float y);
    void set_vec3(std::string_view name, const glm::vec3& val);
    void set_vec3(std::string_view name, float x, float y, float z);
    void set_vec4(std::string_view name, const glm::vec4& val);
    void set_vec4(std::string_view name, float x, float y, float z, float w);
    void set_mat2(std::string_view name, const glm::mat2& val);
    void set_mat3(std::string_view name, const glm::mat3& val);
    void set_mat4(std::string_view name, const glm::mat4& val);

    void use() const { glUseProgram(id_); };

    void dispatch(std::uint32_t x, std::uint32_t y, std::uint32_t z) const {
        glDispatchCompute(x, y, z);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

private:
    Shader(const std::vector<std::pair<GLuint, std::filesystem::path>>& pairs,
           const std::map<std::string, std::any>& defines);

    [[nodiscard]] auto uniform_location(std::string_view name) -> int;

    static std::string insert_defines(
        const std::filesystem::path& path, const std::string& source,
        const std::map<std::string, std::any>& defines);

    static std::string insert_includes(const std::filesystem::path& path,
                                       const std::string& source);

    static GLuint compile(GLuint type, const std::filesystem::path& path,
                          const std::string& source);

    static GLuint link(const std::vector<GLuint>& ids);

    static std::string shader_type_to_string(GLuint shader_type);

private:
    std::unordered_map<std::string_view, int> uniform_cache_;
    std::uint32_t id_{};
};

}  // namespace gl