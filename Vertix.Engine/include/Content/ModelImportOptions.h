//
// Created by Natsurainko on 2026/3/4.
//

#ifndef VERTIX_MODELIMPORTOPTIONS_H
#define VERTIX_MODELIMPORTOPTIONS_H

#include <assimp/postprocess.h>

namespace Vertix::Engine {
    struct ModelImportOptions {
        bool ApplyTransformationToModel = true;
        unsigned int AssimpPostProcessSteps =
                aiProcess_Triangulate |
                aiProcess_GenSmoothNormals |
                aiProcess_CalcTangentSpace |
                aiProcess_MakeLeftHanded |
                aiProcess_OptimizeMeshes;
    };
}

#endif //VERTIX_MODELIMPORTOPTIONS_H