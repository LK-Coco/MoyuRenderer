#include "shader.h"
#include "glad/glad.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace MR {

bool check_compile_error(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
    return success;
}

void read_shader_file(const char* path, std::string& code) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
}

Shader::Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path) {
    std::string vertex_code;
    std::string fragment_code;
    std::string geometry_code;
    try {
        read_shader_file(vertex_path, vertex_code);
        read_shader_file(fragment_path, fragment_code);
        if (geometry_path != nullptr) {
            read_shader_file(fragment_path, geometry_code);
        }
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        std::cout << vertex_path << std::endl;
    }
    const char* v_shader_code = vertex_code.c_str();
    const char* f_shader_code = fragment_code.c_str();
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, NULL);
    glCompileShader(vertex);
    check_compile_error(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, NULL);
    glCompileShader(fragment);
    check_compile_error(fragment, "FRAGMENT");
    unsigned int geometry;
    if (geometry_path != nullptr) {
        const char* gShaderCode = geometry_code.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        check_compile_error(geometry, "GEOMETRY");
    }
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    if (geometry_path != nullptr) glAttachShader(id_, geometry);
    glLinkProgram(id_);
    check_compile_error(id_, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry_path != nullptr) glDeleteShader(geometry);

    int num_attributes, num_uniform;
    glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &num_attributes);
    glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &num_uniform);

    char buffer[128];
    for (unsigned int i = 0; i < num_attributes; ++i) {
        GLenum glType;
        int size;
        glGetActiveAttrib(id_, i, sizeof(buffer), 0, &size, &glType, buffer);
        std::string name = std::string(buffer);
        attributes_[name].name = name;
        attributes_[name].size = size;
        attributes_[name].type = UniformType::u_bool;
        attributes_[name].location = glGetAttribLocation(id_, buffer);
    }

    for (unsigned int i = 0; i < num_uniform; ++i) {
        GLenum glType;
        int size;
        glGetActiveUniform(id_, i, sizeof(buffer), 0, &size, &glType, buffer);
        std::string name = std::string(buffer);
        uniforms_[name].name = name;
        uniforms_[name].type = UniformType::u_bool;
        uniforms_[name].size = size;
        uniforms_[name].location = glGetUniformLocation(id_, buffer);
    }

    std::cout << "shader init ok" << std::endl;
}

void Shader::use() { glUseProgram(id_); }

bool Shader::has_uniform(const std::string& name) {
    return uniforms_.find(name) != uniforms_.end();
}

void Shader::set_bool(const std::string& name, bool val) {
    if (has_uniform(name)) glUniform1i(uniforms_[name].location, (int)val);
}

void Shader::set_int(const std::string& name, int val) {
    if (has_uniform(name)) glUniform1i(uniforms_[name].location, val);
}

void Shader::set_float(const std::string& name, float val) {
    if (has_uniform(name)) glUniform1f(uniforms_[name].location, val);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& val) {
    if (has_uniform(name)) glUniform2fv(uniforms_[name].location, 1, &val[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) {
    if (has_uniform(name)) glUniform2f(uniforms_[name].location, x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& val) {
    if (has_uniform(name)) glUniform3fv(uniforms_[name].location, 1, &val[0]);
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) {
    if (has_uniform(name)) glUniform3f(uniforms_[name].location, x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& val) {
    if (has_uniform(name)) glUniform4fv(uniforms_[name].location, 1, &val[0]);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) {
    if (has_uniform(name)) glUniform4f(uniforms_[name].location, x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& val) {
    if (has_uniform(name)) glUniformMatrix2fv(uniforms_[name].location, 1, GL_FALSE, &val[0][0]);
}

void Shader::set_mat3(const std::string& name, const glm::mat3& val) {
    if (has_uniform(name)) glUniformMatrix3fv(uniforms_[name].location, 1, GL_FALSE, &val[0][0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& val) {
    if (has_uniform(name)) glUniformMatrix4fv(uniforms_[name].location, 1, GL_FALSE, &val[0][0]);
}

}  // namespace MR