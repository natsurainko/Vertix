//
// Created by Natsurainko on 2026/5/14.
//

#pragma once

#include "RenderResource.h"

namespace Vertix {
    class RenderBuffer : public RenderResource {
    public:
        RenderBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES                   currentResourceState)
        : RenderResource(RenderResourceKind::Buffer, d3d12Resource, currentResourceState) {}
    };
}
