//
// Created by Natsurainko on 2026/5/5.
//

#ifndef VERTIX_RENDERTEXTUREACCESSOR_H
#define VERTIX_RENDERTEXTUREACCESSOR_H

namespace Vertix {
    enum RenderResourceAccessor {
        RenderTarget    = 0x1,
        DepthStencil    = 0x2,
        UnorderedAccess = 0x4,
        ShaderResource  = 0x8,

        ConstantBufferAccessor = 0x10,
        VertexBufferAccessor   = 0x20,
        IndexBufferAccessor    = 0x40,

        AccelerationStructure = 0x80,

        DrawColorSampleAccessor         = RenderTarget    | ShaderResource,
        DrawDepthSampleAccessor         = DepthStencil    | ShaderResource,
        UnorderedAccessSampleAccessor   = UnorderedAccess | ShaderResource,
    };
}

#endif //VERTIX_RENDERTEXTUREACCESSOR_H
