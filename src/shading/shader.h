#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "shading_type.h"

namespace MR {

enum class ShaderCompileType {
    Vertex,
    Fragment,
    Geometry,
    Program,
    Compute,
};

class Shader {
public:
    Shader() : id_(-1){};
    Shader(const char* vertex_path, const char* fragment_path,
           const char* geometry_path = nullptr);
    Shader(const char* comp_path);
    virtual ~Shader() {}

    unsigned int get_id() const { return id_; }

    void use();

    void dispatch(unsigned int x, unsigned int y, unsigned int z) const;

    // bool has_uniform(const std::string& name);
    // inline const std::unordered_map<std::string, Uniform>& get_unifrom()
    // const {
    //     return uniforms_;
    // }

    void set_bool(const std::string& name, bool val) const;
    void set_int(const std::string& name, int val) const;
    void set_float(const std::string& name, float val) const;
    void set_vec2(const std::string& name, const glm::vec2& val) const;
    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, const glm::vec3& val) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, const glm::vec4& val) const;
    void set_vec4(const std::string& name, float x, float y, float z,
                  float w) const;
    void set_mat2(const std::string& name, const glm::mat2& val) const;
    void set_mat3(const std::string& name, const glm::mat3& val) const;
    void set_mat4(const std::string& name, const glm::mat4& val) const;

private:
    unsigned int id_;

    // std::unordered_map<std::string, Uniform> uniforms_;
    // std::unordered_map<std::string, VertexAttribute> attributes_;
};

}  // namespace MR