//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RAYTRACINGSTRUCTURE_H
#define VERTIX_RAYTRACINGSTRUCTURE_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "VERTIX_EXPORT.h"

namespace Vertix {
    class Mesh;
    class BottomLevelAccelerationStructure {
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> BLASResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> BLASScratchResource;

        static VERTIX_API BottomLevelAccelerationStructure* Create(
            const Microsoft::WRL::ComPtr<ID3D12Device5> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList,
            Mesh* mesh);
    };
}

#endif //VERTIX_RAYTRACINGSTRUCTURE_H