using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using System.Numerics;
using Vertix.Graphics;

namespace Vertix.Rendering;

public interface IRenderTarget : IDisposable
{
    Vector2D<uint> Size { get; }

    IReadOnlyList<ITexture> TargetTextures { get; }

    bool Initialized { get; }

    void Initialize();

    void BindRenderTarget();

    void AttachTargetTexture(ITexture texture, RenderTargetAttachment renderTargetAttachment = RenderTargetAttachment.Color, int mipmapIndex = 0);

    void ClearTargetTexture(ClearBufferMask buffers, int index, Vector4 color = default, float depth = 1f, int stencil = 0);
}