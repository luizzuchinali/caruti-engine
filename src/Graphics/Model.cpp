#include "Model.hpp"

namespace Graphics {

    void Graphics::Model::Draw(Graphics::Shader &shader) {
        for (auto &Meshe: Meshes)
            Meshe.Draw(shader);
    }

    void Graphics::Model::LoadModel(const std::string &path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            auto error =  import.GetErrorString();
            Log::Error("ASSIMP: {}", error);
            return;
        }

        Directory = path.substr(0, path.find_last_of('/'));

//        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
//            aiMaterial *material = scene->mMaterials[i];
//            fmt::print("{}\n", material->GetName().C_Str());
//
//            for (unsigned int j = 0; j < aiTextureType_UNKNOWN; j++) {
//                if (material->GetTextureCount((aiTextureType) j) > 0) {
//                    aiString texture_file;
//                    material->Get(AI_MATKEY_TEXTURE((aiTextureType) j, 0), texture_file);
//
//                    if (auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {
//                        //returned pointer is not null, read texture from memory
//                        fmt::print("Embedded {}\n", texture->mHeight);
//                    } else {
//                        //regular file, check if it exists and read it
//                        fmt::print("Regular {}\n", texture_file.C_Str());
//                    }
//                }
//            }
//        }

        ProcessNode(scene->mRootNode, scene);
    }

    void Graphics::Model::ProcessNode(aiNode *node, const aiScene *scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            Meshes.push_back(ProcessMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Graphics::Mesh Graphics::Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<TextureIdentifier> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            glm::vec3 vec;
            vec.x = mesh->mVertices[i].x;
            vec.y = mesh->mVertices[i].y;
            vec.z = mesh->mVertices[i].z;
            vertex.Position = vec;

            vec.x = mesh->mNormals[i].x;
            vec.y = mesh->mNormals[i].y;
            vec.z = mesh->mNormals[i].z;
            vertex.Normal = vec;

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec2;
                vec2.x = mesh->mTextureCoords[0][i].x;
                vec2.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec2;
            } else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<TextureIdentifier> diffuseMaps =
                    LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<TextureIdentifier> specularMaps =
                    LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return {vertices, indices, textures};
    }

    std::vector<Graphics::TextureIdentifier>
    Graphics::Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName) {
        std::vector<TextureIdentifier> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (auto &j: TexturesLoaded) {
                if (std::strcmp(j.Path.data(), str.C_Str()) == 0) {
                    textures.push_back(j);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                TextureIdentifier texture;
                texture.Id = TextureFromFile(str.C_Str(), Directory, false);
                texture.Type = typeName;
                texture.Path = str.C_Str();
                textures.push_back(texture);
                TexturesLoaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int Graphics::Model::TextureFromFile(const char *path, const std::string &directory, bool gamma) {
        stbi_set_flip_vertically_on_load(true);

        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

        if (data) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            Log::Error("TEXTURE: Texture failed to load at path {} ", path);
            stbi_image_free(data);
        }

        return textureID;
    }

}