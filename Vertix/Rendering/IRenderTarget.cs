using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using Vertix.Graphics;

namespace Vertix.Rendering;

public interface IRenderTarget : IDisposable
{
    Vector2D<uint> Size { get; }

    IReadOnlyList<ITexture> TargetTextures { get; }

    bool Initialized { get; }

    void Initialize();

    void BindRenderTarget();

    void AttachTargetTexture(ITexture texture, RenderTargetAttachment renderTargetAttachment = RenderTargetAttachment.Color);
}