//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODELIMPORTER_H
#define VERTIX_MODELIMPORTER_H

#include <string>
#include <assimp/scene.h>

#include "Primitive/Model.h"

namespace Vertix::Engine {
    class ModelImporter {
    public:
        [[nodiscard]]
        static bool TryLoadFromFile(Model &model, const std::string &filePath);

    private:
        static void ProcessNode(Model &model,
                                const aiNode* node,
                                const aiScene* scene);

        [[nodiscard]]
        static Mesh ProcessMesh(const aiMesh *aiMesh);
    };
}

#endif //VERTIX_MODELIMPORTER_H