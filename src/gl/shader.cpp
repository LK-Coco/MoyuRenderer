#include "shader.h"
#include "gl_exception.h"

#include <regex>

namespace gl {

Shader::Shader(const std::filesystem::path& vertex_path,
               const std::filesystem::path& fragment_path,
               const std::map<std::string, std::any>& defines)
    : Shader(
          std::vector<std::pair<GLuint, std::filesystem::path>>{
              {GL_VERTEX_SHADER, vertex_path},
              {GL_FRAGMENT_SHADER, fragment_path}},
          defines) {}

Shader::Shader(const std::filesystem::path& vertex_path,
               const std::filesystem::path& fragment_path,
               const std::filesystem::path& geometry_path,
               const std::map<std::string, std::any>& defines)
    : Shader(
          std::vector<std::pair<GLuint, std::filesystem::path>>{
              {GL_VERTEX_SHADER, vertex_path},
              {GL_FRAGMENT_SHADER, fragment_path},
              {GL_GEOMETRY_SHADER, geometry_path}},
          defines) {}

Shader::Shader(const std::filesystem::path& comp_path,
               const std::map<std::string, std::any>& defines)
    : Shader(
          std::vector<std::pair<GLuint, std::filesystem::path>>{
              {GL_COMPUTE_SHADER, comp_path}},
          defines) {}

Shader::Shader(
    const std::vector<std::pair<GLuint, std::filesystem::path>>& pairs,
    const std::map<std::string, std::any>& defines) {
    std::vector<GLuint> ids;
    for (int i = 0; i < pairs.size(); i++) {
        auto path = pairs[i].second;
        std::optional<std::string> source = util::read_file(path);
        if (!source.has_value()) throw FileLoadError(path, "Shader");
        source = insert_includes(path, source.value());
        source = insert_defines(path, source.value(), defines);
        ids.push_back(compile(pairs[i].first, path, source.value()));
    }
    id_ = link(ids);
}

static std::string insert_defines(
    const std::filesystem::path& path, const std::string& source,
    const std::map<std::string, std::any>& defines) {
    std::stringstream ss;
    for (auto kv : defines) {
        std::string key = kv.first;
        std::any value = kv.second;
        std::string value_str;
        if (value.type() == typeid(int32_t)) {
            value_str = std::to_string(std::any_cast<int32_t>(value));
        } else if (value.type() == typeid(uint32_t)) {
            value_str = std::to_string(std::any_cast<uint32_t>(value));
        } else if (value.type() == typeid(float)) {
            value_str = std::to_string(std::any_cast<float>(value));
        } else if (value.type() == typeid(double)) {
            value_str = std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(bool)) {
            value_str = std::any_cast<bool>(value) ? "true" : "false";
        } else if (value.type() == typeid(std::string)) {
            value_str = std::any_cast<std::string>(value);
        } else {
            throw ShaderException("define value type not supported! ");
        }
        ss << std::format("#define {} {}\n", key, value_str);
    }

    std::string new_source = ss.str() + source;

    auto regex = std::regex("(?:^|\n)\\s*(#version.*\n)");
    std::smatch sm;
    std::regex_search(new_source, sm, regex);
    if (!(sm.size() >= 2)) {
        throw ShaderException("GLSL version string not found");
    }
    auto version_str = sm[1].str();

    new_source = new_source.replace(new_source.find(version_str),
                                    version_str.size(), "");
    new_source = version_str + new_source;

    return new_source;
}

static std::string insert_includes(const std::filesystem::path& path,
                                   const std::string& source) {
    auto regex = std::regex("(?:^|\n)\\s*#include\\s+\"(.+)\"\n");
    std::smatch sm;

    std::string new_source = source;
    while (true) {
        std::regex_search(new_source, sm, regex);
        if (sm.size() < 2) return new_source;

        std::filesystem::path include_path = sm[1].str();
        std::optional<std::string> content =
            util::read_file(include_path);  // TODO need add shader search dic
        if (!content.has_value()) {
            throw FileLoadError(include_path, "Shader");
        }

        std::string include_str = std::format("#include \"{}\"", sm[1].str());
        new_source = new_source.replace(new_source.find(include_str),
                                        include_str.size(), content.value());
    }

    return new_source;
}

GLuint Shader::compile(GLuint type, const std::filesystem::path& path,
                       const std::string& source) {
    GLuint shader_id = glCreateShader(type);
    const char* temp = source.c_str();
    glShaderSource(shader_id, 1, &temp, nullptr);
    glCompileShader(shader_id);
    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success) return shader_id;

    int length;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
    char* log = new char[length];
    glGetShaderInfoLog(shader_id, length, nullptr, log);
    std::string log_str = log;
    delete[] log;
    throw ShaderCompileError(shader_type_to_string(type), path, log_str);
}

GLuint Shader::link(const std::vector<GLuint>& ids) {
    GLuint program_id = glCreateProgram();

    for (auto id : ids) glAttachShader(program_id, id);
    glLinkProgram(program_id);
    for (auto id : ids) glDeleteShader(id);

    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (success) return program_id;

    int length;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
    char* log = new char[length];
    glGetProgramInfoLog(program_id, length, nullptr, log);
    std::string log_str = log;
    delete[] log;
    throw ShaderException(log_str);
}

std::string Shader::shader_type_to_string(GLuint shader_type) {
    if (shader_type == GL_VERTEX_SHADER)
        return "Vertex";
    else if (shader_type == GL_FRAGMENT_SHADER)
        return "Fragment";
    else if (shader_type == GL_GEOMETRY_SHADER)
        return "Geometry";
    else
        return "Compute";
}

auto Shader::uniform_location(std::string_view name) -> int {
    if (uniform_cache_.find(name) != uniform_cache_.end()) [[likely]] {
        return uniform_cache_[name];
    } else {
        const int location{glGetUniformLocation(id_, name.data())};
        uniform_cache_[name] = location;
        return location;
    }
}

void Shader::set_bool(std::string_view name, bool val) {
    glUniform1i(uniform_location(name), (int)val);
}

void Shader::set_int(std::string_view name, int val) {
    glUniform1i(uniform_location(name), val);
}

void Shader::set_float(std::string_view name, float val) {
    glUniform1f(uniform_location(name), val);
}

void Shader::set_vec2(std::string_view name, const glm::vec2& val) {
    glUniform2fv(uniform_location(name), 1, &val[0]);
}

void Shader::set_vec2(std::string_view name, float x, float y) {
    glUniform2f(uniform_location(name), x, y);
}

void Shader::set_vec3(std::string_view name, const glm::vec3& val) {
    glUniform3fv(uniform_location(name), 1, &val[0]);
}

void Shader::set_vec3(std::string_view name, float x, float y, float z) {
    glUniform3f(uniform_location(name), x, y, z);
}

void Shader::set_vec4(std::string_view name, const glm::vec4& val) {
    glUniform4fv(uniform_location(name), 1, &val[0]);
}

void Shader::set_vec4(std::string_view name, float x, float y, float z,
                      float w) {
    glUniform4f(uniform_location(name), x, y, z, w);
}

void Shader::set_mat2(std::string_view name, const glm::mat2& val) {
    glUniformMatrix2fv(uniform_location(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::set_mat3(std::string_view name, const glm::mat3& val) {
    glUniformMatrix3fv(uniform_location(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::set_mat4(std::string_view name, const glm::mat4& val) {
    glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, &val[0][0]);
}

}  // namespace gl