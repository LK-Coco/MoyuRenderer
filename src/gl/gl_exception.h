#pragma once

#include <filesystem>

namespace gl {

class FileLoadError : public std::exception {
public:
    FileLoadError() = delete;
    FileLoadError(const std::filesystem::path& path, const std::string& module);
    const char* what() const override;

private:
    std::string message_;
};

class ShaderException : public std::exception {
public:
    ShaderException() = delete;
    ShaderException(const std::string& error);
    const char* what() const override;

private:
    std::string message_;
};

class ShaderCompileError : public std::exception {
public:
    ShaderCompileError() = delete;
    ShaderCompileError(const std::string& compile_type,
                       const std::filesystem::path& path,
                       const std::string& error_log);
    const char* what() const override;

private:
    std::string message_;
};

}  // namespace gl