#pragma once

#include <optional>
#include <string>

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
    static std::optional<std::string> get_file_path(const char* file_extern);

    static std::optional<std::string> imgui_image_button(
        const std::string& image_name, const std::string& display_name);
};

}  // namespace MR
