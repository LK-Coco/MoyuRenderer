#pragma once

#include <string>
#include <vector>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"
#include "texture.h"
#include "object.h"

namespace MR {

class Model : public Object {
public:
    Model(std::string file_path);

    virtual void render(std::shared_ptr<Shader>& shader) override;

private:
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type,
                                                std::string type_name);

    std::string directory_;
    std::vector<Mesh> meshes_;
    std::vector<Texture> textures_loaded_;
};

}  // namespace MR