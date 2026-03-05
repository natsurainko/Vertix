//
// Created by Natsurainko on 2026/3/4.
//

#ifndef VERTIX_MODELIMPORTOPTIONS_H
#define VERTIX_MODELIMPORTOPTIONS_H

#include <assimp/postprocess.h>

namespace Vertix::Engine {
    struct ModelImportOptions {
        bool CombineIntoSingleMesh;
        unsigned int AssimpPostProcessSteps;

        [[nodiscard]]
        static ModelImportOptions GetDefaultModelImportOptions() {
            ModelImportOptions options{};
            options.CombineIntoSingleMesh = true;
            options.AssimpPostProcessSteps =
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_CalcTangentSpace;

            return options;
        }
    };
}

#endif //VERTIX_MODELIMPORTOPTIONS_H