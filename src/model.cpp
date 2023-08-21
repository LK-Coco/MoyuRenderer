#include "model.h"

namespace MR {

Model::Model(std::string file_path) {
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                         aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        // cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    directory_ = file_path.substr(0, file_path.find_last_of('/'));
}

void Model::process_node(aiNode* node, const aiScene* scene) {
    for (int i = 0; i < node->mNumMeshes; ++i) {
        auto mesh = process_mesh(scene->mMeshes[i], scene);
        meshes_.push_back(mesh);
    }
}
Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        glm::vec3 pos{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.position = pos;
        glm::vec3 n{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        vertex.normal = n;
        if (mesh->HasTextureCoords(0)) {
            glm::vec2 uv{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            vertex.tex_coords = uv;
        } else {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }
        glm::vec3 tangent{mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
        vertex.tangent = tangent;
        glm::vec3 bt{mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
        vertex.bitangent = bt;
        vertices.push_back(vertex);
    }
    for (int i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuse_maps =
        load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<Texture> specular_maps =
        load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    std::vector<Texture> normal_maps =
        load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

    std::vector<Texture> height_maps =
        load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), height_maps.begin(), height_maps.end());

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type,
                                                   std::string type_name) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded_.size(); j++) {
            if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded_[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            std::string file_name = std::string(str.C_Str());
            file_name = directory_ + '/' + file_name;
            Texture texture(file_name.c_str());
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded_.push_back(texture);
        }
    }
    return textures;
}

}  // namespace MR