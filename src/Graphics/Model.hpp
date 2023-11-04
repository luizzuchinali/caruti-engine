#pragma once

#include "Mesh.hpp"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Graphics {
    class Model {
    public:
        explicit Model(const char *path) {
            LoadModel(path);

//            for (auto &tex: TexturesLoaded) {
//                fmt::print("Id {} ", tex.Id);
//                fmt::print("Type {} ", tex.Type);
//                fmt::print("Path {}\n", tex.Path);
//            }
        }

        void Draw(Graphics::Shader &shader);

    private:
        std::vector<TextureIdentifier> TexturesLoaded;

        std::vector<Mesh> Meshes;
        std::string Directory;

        void LoadModel(const std::string &path);

        void ProcessNode(aiNode *node, const aiScene *scene);

        Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<TextureIdentifier>
        LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);

        static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma);
    };
}

