#include <Windows.h>
#include <optional>
#include "utility.h"
#include "resources/resources.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace MR {

std::optional<std::string> Utils::get_file_path(const char* file_extern) {
    TCHAR szBuffer[MAX_PATH] = {0};
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = file_extern;  // 要选择的文件后缀
    ofn.lpstrFile = szBuffer;       // 存放文件的缓冲区
    ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
    ofn.nFilterIndex = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                OFN_EXPLORER;  // 标志如果是多选要加上OFN_ALLOWMULTISELECT
    BOOL bSel = GetOpenFileName(&ofn);

    if (bSel) {
        return std::make_optional(szBuffer);
    } else {
        return std::nullopt;
    }
}

std::optional<std::string> Utils::imgui_image_button(
    const std::string& image_name, const std::string& display_name) {
    ImGui::TextWrapped("%s", display_name.c_str());
    ImVec2 size{100, 100};
    auto map = Resources::get_texture(image_name);
    ImTextureID id = map == nullptr ? 0 : (ImTextureID)(map->id);
    if (ImGui::ImageButton(id, size)) {
        return get_file_path("image files\0*.jpg;*.png\0");
    }

    return std::nullopt;
}

}  // namespace MR