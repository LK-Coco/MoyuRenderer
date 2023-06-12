#include "model.h"
#include "obj_loader.h"

namespace MR {

Model::Model(const std::string& file_path) {
    objl::Loader loader;
    bool load_out = loader.LoadFile(file_path);
    if (!load_out) throw "load obj failed!";

    for (auto mesh : loader.LoadedMeshes) {
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            Triangle* t = new Triangle();
            for (int j = 0; j < 3; ++j) {
                auto vertic = mesh.Vertices[i + j];
                t->set_vertex(
                    j, glm::vec4(vertic.Position.X, vertic.Position.Y, vertic.Position.Z, 1.0f));
                t->set_normal(j, glm::vec3(vertic.Normal.X, vertic.Normal.Y, vertic.Normal.Z));
                t->set_tex_coord(j,
                                 glm::vec2(vertic.TextureCoordinate.X, vertic.TextureCoordinate.Y));
            }
            triangle_list.push_back(t);
        }
    }
}

}  // namespace MR