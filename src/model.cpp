#include "model.h"
#include <iostream>
#include "resources/resources.h"

namespace MR {

Model::Model(std::string file_path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        file_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                       aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString()
                  << std::endl;
        return;
    }

    directory_ = file_path.substr(0, file_path.find_last_of('/'));

    process_node(scene->mRootNode, scene);

    std::cout << "model load ok!   mesh:" << meshes_.size() << std::endl;
}

void Model::render(std::shared_ptr<Material>& mat) {
    for (int i = 0; i < meshes_.size(); ++i) {
        meshes_[i].draw(mat);
    }
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    for (int i = 0; i < node->mNumMeshes; ++i) {
        auto mesh = process_mesh(scene->mMeshes[i], scene);
        meshes_.push_back(mesh);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}
Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        glm::vec3 pos{mesh->mVertices[i].x, mesh->mVertices[i].y,
                      mesh->mVertices[i].z};
        vertex.position = pos;
        glm::vec3 n{mesh->mNormals[i].x, mesh->mNormals[i].y,
                    mesh->mNormals[i].z};
        vertex.normal = n;
        if (mesh->mTextureCoords[0]) {
            glm::vec2 uv{mesh->mTextureCoords[0][i].x,
                         mesh->mTextureCoords[0][i].y};
            vertex.tex_coords = uv;
            glm::vec3 tangent{mesh->mTangents[i].x, mesh->mTangents[i].y,
                              mesh->mTangents[i].z};
            vertex.tangent = tangent;
            glm::vec3 bt{mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                         mesh->mBitangents[i].z};
            vertex.bitangent = bt;
        } else {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    for (int i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // load_material_textures(material, aiTextureType_DIFFUSE,
    // "texture_diffuse");

    // load_material_textures(material, aiTextureType_SPECULAR,
    //                        "texture_specular");

    // load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");

    // load_material_textures(material, aiTextureType_AMBIENT,
    // "texture_height");

    return Mesh(vertices, indices, textures);
}

void Model::load_material_textures(aiMaterial* mat, aiTextureType type,
                                   std::string type_name) {
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string tex_name = type_name + std::to_string(i);
        std::string file_path = directory_ + '/' + std::string(str.C_Str());
        std::cout << "load texture:" << file_path << "    name:" << tex_name
                  << std::endl;
        Resources::load_texture(tex_name, file_path);

        // bool skip = false;
        // for (unsigned int j = 0; j < textures_loaded_.size(); j++) {
        //     if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) ==
        //         0) {
        //         textures.push_back(textures_loaded_[j]);
        //         skip = true;
        //         break;
        //     }
        // }
        // if (!skip) {
        //     std::string file_name = std::string(str.C_Str());
        //     file_name = directory_ + '/' + file_name;
        //     Texture texture(file_name.c_str());
        //     texture.type = type_name;
        //     texture.path = str.C_Str();
        //     textures.push_back(texture);
        //     textures_loaded_.push_back(texture);
        // }
    }
}

}  // namespace MR