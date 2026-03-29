//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_TOPLEVELACCELERATIONSTRUCTURE_H
#define VERTIX_TOPLEVELACCELERATIONSTRUCTURE_H

#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "VERTIX_EXPORT.h"

namespace Vertix {
    class TopLevelAccelerationStructure {
    public:
        Microsoft::WRL::ComPtr<ID3D12Resource> TLASResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> TLASScratchResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> InstancesBufferResource;

        static VERTIX_API TopLevelAccelerationStructure* Create(
            const Microsoft::WRL::ComPtr<ID3D12Device5> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList,
            const std::vector<D3D12_RAYTRACING_INSTANCE_DESC> &instanceDescs);
    };
}

#endif //VERTIX_TOPLEVELACCELERATIONSTRUCTURE_H