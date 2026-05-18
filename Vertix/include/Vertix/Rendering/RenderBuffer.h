//
// Created by Natsurainko on 2026/5/14.
//

#ifndef VERTIX_RENDERBUFFER_H
#define VERTIX_RENDERBUFFER_H

#include "RenderResource.h"

namespace Vertix {
    enum class RenderBufferUsage {
        None,

        ConstantBuffer,
        StructuredBuffer,
        CounterBuffer,

        AccelerationStructure
    };

    class RenderBuffer : public RenderResource {
    public:
        RenderBuffer(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState)
            : RenderResource(RenderResourceKind::Buffer, d3d12Resource, currentResourceState) {}
    };
}

#endif //VERTIX_RENDERBUFFER_H
