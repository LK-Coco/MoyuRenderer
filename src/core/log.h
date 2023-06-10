#pragma once

#include "base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
// #pragma warning(push, 0)
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

// #pragma warning(pop)
namespace MR {

class Log {
public:
    static void Init();

    static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};

}  // namespace MR

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) {
    return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) {
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion) {
    return os << glm::to_string(quaternion);
}

// Core log macros
#define MR_CORE_TRACE(...) ::MR::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MR_CORE_INFO(...) \
    SPDLOG_LOGGER_CALL(::MR::Log::GetCoreLogger(), spdlog::level::level_enum::info, __VA_ARGS__)
#define MR_CORE_WARN(...) ::MR::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MR_CORE_ERROR(...) ::MR::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MR_CORE_CRITICAL(...) ::MR::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MR_TRACE(...) ::MR::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MR_INFO(...) ::MR::Log::GetClientLogger()->info(__VA_ARGS__)
#define MR_WARN(...) ::MR::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MR_ERROR(...) ::MR::Log::GetClientLogger()->error(__VA_ARGS__)
#define MR_CRITICAL(...) ::MR::Log::GetClientLogger()->critical(__VA_ARGS__)
