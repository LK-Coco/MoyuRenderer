#pragma once

#include <string>

namespace MR {

/*
.scenemeta 文件格式说明

[object]
assets/AfricanHead/african_head.obj 1.0,1.0,1.0 0.0,0.0,0.0 1.0,1.0,1.0  //
文件路径 位置 旋转 缩放
...
[light]
point 0.1,0.7,0.2,1.0 5,0,5 // 灯光类型 颜色 位置
[material]
pbr
--albedoMap:path
--normalMap:path
--metallicMap:path
--roughnessMap:path
--aoMap:path

*/

class SceneMeta {
public:
    SceneMeta(const std::string& file_path);

public:
    std::string material_type;
};

}  // namespace MR