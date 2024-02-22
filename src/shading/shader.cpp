#include "shader.h"
#include "glad/glad.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace MR {

UniformType gl_uniform_type_to_uniform_type(GLenum gltype) {
    switch (gltype) {
        case GL_FLOAT: return UniformType::u_float;
        case GL_INT: return UniformType::u_int;
        case GL_BOOL: return UniformType::u_bool;
        case GL_FLOAT_VEC2: return UniformType::u_vec2;
        case GL_FLOAT_VEC3: return UniformType::u_vec3;
        case GL_FLOAT_MAT2: return UniformType::u_mat2;
        case GL_FLOAT_MAT3: return UniformType::u_mat3;
        case GL_FLOAT_MAT4: return UniformType::u_mat4;
        case GL_SAMPLER_1D: return UniformType::u_sampler_1d;
        case GL_SAMPLER_2D: return UniformType::u_sampler_2d;
        case GL_SAMPLER_3D: return UniformType::u_sampler_3d;
        case GL_SAMPLER_CUBE: return UniformType::u_sampler_cube;
        default: return UniformType::u_bool;
    }
}

bool check_compile_error(GLuint shader, const std::string& type,
                         const char* file) {
    GLint success;
    GLchar infoLog[1024];
    if (type == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout
                << file << " -- ERROR::PROGRAM_LINKING_ERROR of type: Program\n"
                << infoLog
                << "\n -- --------------------------------------------------- "
                   "-- "
                << std::endl;
        }
    } else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout
                << file
                << " -- ERROR::SHADER_COMPILATION_ERROR of type: " << type
                << "\n"
                << infoLog
                << "\n -- --------------------------------------------------- "
                   "-- "
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

Shader::Shader(const char* vertex_path, const char* fragment_path,
               const char* geometry_path) {
    std::string vertex_code;
    std::string fragment_code;
    std::string geometry_code;
    try {
        read_shader_file(vertex_path, vertex_code);
        read_shader_file(fragment_path, fragment_code);
        if (geometry_path != nullptr) {
            read_shader_file(geometry_path, geometry_code);
        }
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
                  << std::endl;
        std::cout << vertex_path << std::endl;
    }
    const char* v_shader_code = vertex_code.c_str();
    const char* f_shader_code = fragment_code.c_str();
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, NULL);
    glCompileShader(vertex);
    check_compile_error(vertex, "VERTEX", vertex_path);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, NULL);
    glCompileShader(fragment);
    check_compile_error(fragment, "FRAGMENT", fragment_path);
    unsigned int geometry;
    if (geometry_path != nullptr) {
        const char* gShaderCode = geometry_code.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        check_compile_error(geometry, "GEOMETRY", geometry_path);
    }
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    if (geometry_path != nullptr) glAttachShader(id_, geometry);
    glLinkProgram(id_);
    check_compile_error(id_, "PROGRAM", "program");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry_path != nullptr) glDeleteShader(geometry);

    // int num_attributes, num_uniform;
    // glGetProgramiv(id_, GL_ACTIVE_ATTRIBUTES, &num_attributes);
    // glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &num_uniform);

    // char buffer[128];
    // for (unsigned int i = 0; i < num_attributes; ++i) {
    //     GLenum gltype;
    //     int size;
    //     glGetActiveAttrib(id_, i, sizeof(buffer), 0, &size, &gltype, buffer);
    //     std::string name = std::string(buffer);
    //     attributes_[name].name = name;
    //     attributes_[name].size = size;
    //     attributes_[name].type = gl_uniform_type_to_uniform_type(gltype);
    //     attributes_[name].location = glGetAttribLocation(id_, buffer);
    // }

    // for (unsigned int i = 0; i < num_uniform; ++i) {
    //     GLenum gltype;
    //     int size;
    //     glGetActiveUniform(id_, i, sizeof(buffer), 0, &size, &gltype,
    //     buffer); std::string name = std::string(buffer); uniforms_[name].name
    //     = name; uniforms_[name].type =
    //     gl_uniform_type_to_uniform_type(gltype); uniforms_[name].size = size;
    //     uniforms_[name].location = glGetUniformLocation(id_, buffer);
    // }

    std::cout << "shader init ok!   " << vertex_path << std::endl;
}

Shader::Shader(const char* comp_path) {
    std::string comp_code;

    read_shader_file(comp_path, comp_code);
    const char* c_comp_code = comp_code.c_str();

    unsigned int comp = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(comp, 1, &c_comp_code, NULL);
    glCompileShader(comp);
    check_compile_error(comp, "COMPUTE", comp_path);

    id_ = glCreateProgram();
    glAttachShader(id_, comp);
    glLinkProgram(id_);
    check_compile_error(id_, "PROGRAM", "program");

    glDeleteShader(comp);
}

void Shader::use() { glUseProgram(id_); }

void Shader::dispatch(unsigned int x, unsigned int y, unsigned int z) const {
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// bool Shader::has_uniform(const std::string& name) {
//     return uniforms_.find(name) != uniforms_.end();
// }

void Shader::set_bool(const std::string& name, bool val) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)val);
}

void Shader::set_int(const std::string& name, int val) const {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)val);
}

void Shader::set_float(const std::string& name, float val) const {
    glUniform1f(glGetUniformLocation(id_, name.c_str()), (int)val);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& val) const {
    glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &val[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(id_, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& val) const {
    glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &val[0]);
}

void Shader::set_vec3(const std::string& name, float x, float y,
                      float z) const {
    glUniform3f(glGetUniformLocation(id_, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& val) const {
    glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &val[0]);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z,
                      float w) const {
    glUniform4f(glGetUniformLocation(id_, name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& val) const {
    glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                       &val[0][0]);
}

void Shader::set_mat3(const std::string& name, const glm::mat3& val) const {
    glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                       &val[0][0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& val) const {
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                       &val[0][0]);
}

}  // namespace MR