#pragma once

#include <optional>
#include <string>
#include "glm/glm.hpp"

namespace MR {

#define SHASH(name) custom_simple_hash(name)

inline unsigned int custom_simple_hash(const std::string& str) {
    unsigned int hash = 0;

    for (auto& it : str) {
        // NOTE(Joey): be sure to use prime numbers
        hash = 37 * hash + 17 * static_cast<char>(it);
    }

    // NOTE(Joey): hash value will automatically wrap around
    return hash;
}
// NOTE(Joey): supports c string literals
inline unsigned int custom_simple_hash(const char* cStr) {
    std::string str(cStr);
    return custom_simple_hash(str);
}

class Utils {
public:
    [[nodiscard]] static inline constexpr bool is_power_of2(uint32_t v) {
        return v && !(v & (v - 1));
    }

    [[nodiscard]] static uint32_t calc_mip_levels(uint32_t size);

    static std::optional<std::string> get_file_path(const char* file_extern);

    static std::optional<std::string> imgui_image_button(
        const std::string& image_name, const std::string& display_name);
};

}  // namespace MR
