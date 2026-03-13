//
// Created by Natsurainko on 2026/1/10.
//

#ifndef VERTIX_MODEL_H
#define VERTIX_MODEL_H

#include <vector>

#include "Mesh.h"
#include "VERTIX_EXPORT.h"

namespace Vertix {
    class GraphicsDevice;
    class GraphicsCommandList;
    class VERTIX_API Model {
    public:
        std::vector<Mesh> Meshes;

        void UploadToGPU(
            const GraphicsDevice* graphicsDevice,
            const GraphicsCommandList* graphicsCommandList,
            ResourceUploadHeap &resourceUploadHeap);

        void Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const;

        void DrawInstanced(
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            UINT instanceCount) const;
    };
}

#endif //VERTIX_MODEL_H