#pragma once

#include <string>
#include "glm/glm.hpp"

namespace MR {

class Shader {
public:
    Shader() : id_(-1){};
    Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
    virtual ~Shader() {}

    void use();

    void set_bool(const std::string& name, bool val) const;
    void set_int(const std::string& name, int val) const;
    void set_float(const std::string& name, float val) const;
    void set_vec2(const std::string& name, const glm::vec2& val) const;
    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, const glm::vec3& val) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, const glm::vec4& val) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;
    void set_mat2(const std::string& name, const glm::mat2& val) const;
    void set_mat3(const std::string& name, const glm::mat3& val) const;
    void set_mat4(const std::string& name, const glm::mat4& val) const;

private:
    unsigned int id_;
};

}  // namespace MR