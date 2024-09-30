#include "gl_exception.h"

#include <format>

namespace gl {

FileLoadError::FileLoadError(const std::filesystem::path& path,
                             const std::string& module) {
    message_ =
        std::format("[{}] File load error: {}", module, path.generic_string());
}

const char* FileLoadError::what() const { return message_.c_str(); }

ShaderException::ShaderException(const std::string& error) {
    message_ = std::format("[Shader] error: {}", error);
}

const char* ShaderException::what() const { return message_.c_str(); }

ShaderCompileError::ShaderCompileError(const std::string& compile_type,
                                       const std::filesystem::path& path,
                                       const std::string& error_log) {
    message_ = std::format("[Shader {}] {} compile error: {}", compile_type,
                           path.generic_string(), error_log);
}

const char* ShaderCompileError::what() const { return message_.c_str(); }

}  // namespace gl