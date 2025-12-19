using Silk.NET.Core.Native;
using Silk.NET.Direct3D11;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using System.Numerics;
using Vertix.Direct3D11.Graphics;
using Vertix.Direct3D11.Helpers;
using Vertix.Graphics;
using Vertix.Rendering;

namespace Vertix.Direct3D11.Rendering;

public unsafe class D3D11RenderTarget(D3D11GraphicsDevice d3D11GraphicsDevice, Vector2D<uint> size) : IRenderTarget
{
    private readonly ComPtr<ID3D11DeviceContext4> _deviceContext = d3D11GraphicsDevice.DeviceContext;
    private readonly ComPtr<ID3D11Device5> _device = d3D11GraphicsDevice.Device;
    private readonly List<ITexture> _targetTextures = [];
    private readonly List<ComPtr<ID3D11RenderTargetView>> _attachingRenderTargets = [];

    internal ID3D11RenderTargetView*[] _renderTargetTextures = [];
    internal ID3D11DepthStencilView* _depthStencilTexture = default;

    public Vector2D<uint> Size { get; } = size;

    public IReadOnlyList<ITexture> TargetTextures => _targetTextures;

    public bool Initialized { get; private set; }

    public unsafe void AttachTargetTexture(ITexture texture, RenderTargetAttachment renderTargetAttachment = RenderTargetAttachment.Color, int mipmapIndex = 0)
    {
        if (texture is not D3D11Texture d3D11Texture)
            throw new InvalidOperationException();

        if (renderTargetAttachment == RenderTargetAttachment.Color)
        {
            RenderTargetViewDesc rtvDesc = new(d3D11Texture.TextureFormat.ToDXGIFormat());
            ComPtr<ID3D11RenderTargetView> comPtr = default;
            switch (d3D11Texture)
            {
                case D3D11Texture2D:
                    rtvDesc.ViewDimension = RtvDimension.Texture2D;
                    rtvDesc.Texture2D = new((uint)mipmapIndex);
                    break;
                default:
                    throw new NotSupportedException();
            }

            SilkMarshal.ThrowHResult(_device.CreateRenderTargetView(d3D11Texture._textureResource, in rtvDesc, ref comPtr));
            _attachingRenderTargets.Add(comPtr);
        }
        else
        { 
            if (_depthStencilTexture != null) throw new InvalidOperationException();

            DepthStencilViewDesc dsvDesc = new(d3D11Texture.TextureFormat.ToDXGIFormat());
            switch (d3D11Texture)
            {
                case D3D11Texture2D:
                    dsvDesc.ViewDimension = DsvDimension.Texture2D;
                    dsvDesc.Texture2D = new((uint)mipmapIndex);
                    break;
                default:
                    throw new NotSupportedException();
            }

            SilkMarshal.ThrowHResult(_device.CreateDepthStencilView(d3D11Texture._textureResource, in dsvDesc, ref _depthStencilTexture));
        }

        _targetTextures.Add(texture);
    }

    public void Initialize()
    {
        if (Initialized)
            throw new InvalidOperationException("RenderTarget already initialized");

        if (_attachingRenderTargets.Count == 0)
            throw new InvalidOperationException("No target textures attached");

        _renderTargetTextures = [.. _attachingRenderTargets];
        _attachingRenderTargets.Clear();

        Initialized = true;
    }

    public void ClearTargetTexture(ClearBufferMask buffers, int index, Vector4 color = default, float depth = 1, int stencil = 0)
    {
        if ((buffers & ClearBufferMask.Color) == ClearBufferMask.Color)
        {
            float* floats = &color.X;
            _deviceContext.ClearRenderTargetView(_renderTargetTextures[index], floats);
        }

        ClearFlag clearFlag = 0;

        if ((buffers & ClearBufferMask.Depth) == ClearBufferMask.Depth)
            clearFlag |= ClearFlag.Depth;
        if ((buffers & ClearBufferMask.Stencil) == ClearBufferMask.Stencil)
            clearFlag |= ClearFlag.Stencil;

        if (clearFlag != 0)
        {
            if (_depthStencilTexture == null || index != 0)
                throw new InvalidOperationException();

            _deviceContext.ClearDepthStencilView(_depthStencilTexture, (uint)clearFlag, depth, (byte)stencil);
        }
    }

    public void Dispose()
    {
        for (int i = 0; i < _renderTargetTextures.Length; i++)
        {
            _renderTargetTextures[i]->Release();
            _renderTargetTextures[i] = null;
        }

        _renderTargetTextures = [];

        if (_depthStencilTexture == null)
        {
            _depthStencilTexture->Release();
            _depthStencilTexture = null;
        }

        _targetTextures.Clear();
    }
}
