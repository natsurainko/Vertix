using Silk.NET.Core.Native;
using Silk.NET.Direct3D11;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using System.Numerics;
using Vertix.Direct3D11.Graphics;
using Vertix.Graphics;
using Vertix.Rendering;

namespace Vertix.Direct3D11.Rendering;

public class D3D11RenderTarget : IRenderTarget
{
    private readonly ComPtr<ID3D11DeviceContext4> _deviceContext;
    private readonly ComPtr<ID3D11Device5> _device;

    public Vector2D<uint> Size { get; }

    public IReadOnlyList<ITexture> TargetTextures => throw new NotImplementedException();

    public bool Initialized { get; private set; }

    public D3D11RenderTarget(D3D11GraphicsDevice d3D11GraphicsDevice, Vector2D<uint> size)
    {
        _device = d3D11GraphicsDevice.Device;
        _deviceContext = d3D11GraphicsDevice.DeviceContext;
        Size = size;
    }

    public void AttachTargetTexture(ITexture texture, RenderTargetAttachment renderTargetAttachment = RenderTargetAttachment.Color, int mipmapIndex = 0)
    {
        if (texture is not D3D11Texture d3D11Texture)
            throw new InvalidOperationException();


    }

    public void Initialize()
    {
        Initialized = true;
    }

    public void BindRenderTarget()
    {

    }

    public void ClearTargetTexture(ClearBufferMask buffers, int index, Vector4 color = default, float depth = 1, int stencil = 0)
    {
        //if ((buffers & ClearBufferMask.Color) == ClearBufferMask.Color)
        //{
        //    _deviceContext.ClearRenderTargetView(, color);
        //}
    }

    public void Dispose()
    {
        throw new NotImplementedException();
    }
}
