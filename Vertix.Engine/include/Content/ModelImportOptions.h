//
// Created by Natsurainko on 2026/3/4.
//

#ifndef VERTIX_MODELIMPORTOPTIONS_H
#define VERTIX_MODELIMPORTOPTIONS_H

#include <assimp/postprocess.h>

namespace Vertix::Engine {
    struct ModelImportOptions {
        bool TreatAssimpNodeAsModel;
        bool ApplyTransformationToModel;

        unsigned int AssimpPostProcessSteps;

        [[nodiscard]]
        static ModelImportOptions GetDefaultModelImportOptions() {
            ModelImportOptions options{};
            options.TreatAssimpNodeAsModel = false;
            options.ApplyTransformationToModel = true;
            options.AssimpPostProcessSteps =
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_CalcTangentSpace |
                aiProcess_MakeLeftHanded |
                aiProcess_OptimizeMeshes;

            return options;
        }
    };
}

#endif //VERTIX_MODELIMPORTOPTIONS_H