#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "shading_type.h"

namespace MR {

class Shader {
public:
    Shader() : id_(-1){};
    Shader(const char* vertex_path, const char* fragment_path,
           const char* geometry_path = nullptr);
    virtual ~Shader() {}

    unsigned int get_id() const { return id_; }

    void use();

    bool has_uniform(const std::string& name);
    inline const std::unordered_map<std::string, Uniform>& get_unifrom() const {
        return uniforms_;
    }

    void set_bool(const std::string& name, bool val);
    void set_int(const std::string& name, int val);
    void set_float(const std::string& name, float val);
    void set_vec2(const std::string& name, const glm::vec2& val);
    void set_vec2(const std::string& name, float x, float y);
    void set_vec3(const std::string& name, const glm::vec3& val);
    void set_vec3(const std::string& name, float x, float y, float z);
    void set_vec4(const std::string& name, const glm::vec4& val);
    void set_vec4(const std::string& name, float x, float y, float z, float w);
    void set_mat2(const std::string& name, const glm::mat2& val);
    void set_mat3(const std::string& name, const glm::mat3& val);
    void set_mat4(const std::string& name, const glm::mat4& val);

private:
    unsigned int id_;

    std::unordered_map<std::string, Uniform> uniforms_;
    std::unordered_map<std::string, VertexAttribute> attributes_;
};

}  // namespace MR