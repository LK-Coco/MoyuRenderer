#include "utility.h"
#include <Windows.h>

namespace MR {

bool Utils::get_file_path(std::string* file_path, const char* file_extern) {
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

    *file_path = szBuffer;

    return bSel;
}

}  // namespace MR