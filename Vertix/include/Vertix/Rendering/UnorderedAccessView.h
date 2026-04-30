//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_UNORDEREDACCESSVIEW_H
#define VERTIX_UNORDEREDACCESSVIEW_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    class GraphicsDevice;
    class UnorderedAccessView {
    public:
        VERTIX_API UnorderedAccessView(
            const GraphicsDevice* graphicsDevice,
            const D3D12_RESOURCE_DESC &uavResourceDesc,
            const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
            const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);

        [[nodiscard]]
        VERTIX_API D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
            D3D12_RESOURCE_STATES before,
            D3D12_RESOURCE_STATES after,
            UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE) const;

        VERTIX_API void CreateShaderResourceView(
            const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc,
            D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) const;

        VERTIX_API void Resize(const Vector2D<UINT> &size);

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetD3D12Resource() const noexcept {
            return d3d12Resource;
        }

        [[nodiscard]]
        const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() const noexcept {
            return uavHandle;
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device;

        D3D12_CPU_DESCRIPTOR_HANDLE uavHandle;
        D3D12_RESOURCE_DESC uavResourceDesc;
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        bool hasUavDesc = false;
    };
}

#endif //VERTIX_UNORDEREDACCESSVIEW_H
