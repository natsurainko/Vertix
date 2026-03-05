//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODELIMPORTER_H
#define VERTIX_MODELIMPORTER_H

#include <string>
#include <assimp/scene.h>

#include "ModelImportOptions.h"
#include "Primitive/Model.h"

namespace Vertix::Engine {
    class ModelImporter {
    public:
        [[nodiscard]]
        static bool TryLoadFromFile(Model &model, const std::string &filePath, const ModelImportOptions &options = ModelImportOptions::GetDefaultModelImportOptions());

    private:
        static void ProcessNode(Model &model,
                                const aiNode* node,
                                const aiScene* scene,
                                const ModelImportOptions &options);

        [[nodiscard]]
        static Mesh ProcessMesh(const aiMesh *aiMesh);
        static void ProcessMesh(const aiMesh *aiMesh, Mesh &mesh);
    };
}

#endif //VERTIX_MODELIMPORTER_H