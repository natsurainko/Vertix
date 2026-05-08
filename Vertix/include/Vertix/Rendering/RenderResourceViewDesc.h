//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERTEXTUREVIEWDESC_H
#define VERTIX_RENDERTEXTUREVIEWDESC_H

#include <d3d12/d3d12.h>

#include "RenderResourceAccessor.h"

namespace Vertix {
    template<RenderResourceAccessor A> struct ViewDescTrait;

    template<> struct ViewDescTrait<RenderTarget>    { using Type = D3D12_RENDER_TARGET_VIEW_DESC;    };
    template<> struct ViewDescTrait<DepthStencil>    { using Type = D3D12_DEPTH_STENCIL_VIEW_DESC;    };
    template<> struct ViewDescTrait<UnorderedAccess> { using Type = D3D12_UNORDERED_ACCESS_VIEW_DESC; };
    template<> struct ViewDescTrait<ShaderResource>  { using Type = D3D12_SHADER_RESOURCE_VIEW_DESC;  };

    template<RenderResourceAccessor A>
    using ViewDescType = ViewDescTrait<A>::Type;
}

#endif //VERTIX_RENDERTEXTUREVIEWDESC_H
