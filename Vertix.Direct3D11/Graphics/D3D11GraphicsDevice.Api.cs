using Silk.NET.Direct3D11;
using Silk.NET.Maths;
using System;
using System.Drawing;
using Vertix.Direct3D11.Rendering;
using Vertix.Graphics;
using Vertix.Rendering;
using ShaderType = Vertix.Rendering.ShaderType;

namespace Vertix.Direct3D11.Graphics;

public partial class D3D11GraphicsDevice : IGraphicsDevice
{
    public bool EnableDepthTest { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
    public bool EnableFaceCulling { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
    public CullFaceMode CullFace { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
    public FaceWindingOrder FaceWindingOrder { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public bool Disposed => throw new NotImplementedException();

    public unsafe void BindRenderTarget(IRenderTarget? renderTarget)
    {
        if (renderTarget == _currentRenderTarget) return;
        if (renderTarget == null)
        {
            _currentRenderTarget = null;
            if (_defaultRenderTarget == null)
                throw new InvalidOperationException("Default render target is not initialized.");

            fixed (ID3D11RenderTargetView** renderTargetViews = _defaultRenderTarget._renderTargetTextures)
            {
                DeviceContext.OMSetRenderTargets
                (
                    (uint)_defaultRenderTarget._renderTargetTextures.Length,
                    renderTargetViews,
                    _defaultRenderTarget._depthStencilTexture
                );
            }
            return;
        }

        if (renderTarget is not D3D11RenderTarget d3d11RenderTarget)
            throw new InvalidOperationException();

        fixed (ID3D11RenderTargetView** renderTargetViews = d3d11RenderTarget._renderTargetTextures)
        {
            DeviceContext.OMSetRenderTargets
            (
                (uint)d3d11RenderTarget._renderTargetTextures.Length,
                renderTargetViews,
                d3d11RenderTarget._depthStencilTexture
            );
        }
    }

    public void BindTexture(uint bindingIndex, ITexture? texture, ShaderType? shaderType = null)
    {
        throw new NotImplementedException();
    }

    public void BindTextureSampler(uint bindingIndex, ITextureSampler? texture)
    {
        throw new NotImplementedException();
    }

    public void Clear(ClearBufferMask buffers, Color color = default, float depth = 1, int stencil = 0)
    {
        D3D11RenderTarget? renderTarget = (_currentRenderTarget ?? _defaultRenderTarget) 
            ?? throw new InvalidOperationException("No render target is currently bound.");

        renderTarget.ClearTargetTexture(buffers, 0, new(color.R / 255f, color.G / 255f, color.B / 255f, color.A / 255f), depth, stencil);
    }

    public IGraphicsBatcher<TInstance> CreateGraphicsBatcher<TInstance>(in IVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 128) where TInstance : unmanaged
    {
        throw new NotImplementedException();
    }

    public IGraphicsBuffer CreateGraphicsBuffer(GraphicsBufferUsage bufferUsage, GraphicsBufferMapAccess bufferMapAccess = GraphicsBufferMapAccess.None) 
        => new D3D11GraphicsBuffer(this, bufferUsage, bufferMapAccess);

    public IRenderTarget CreateRenderTarget(Vector2D<uint> size) => new D3D11RenderTarget(this, size);

    public IShaderProgram CreateShaderProgram()
    {
        throw new NotImplementedException();
    }

    public ITexture2D CreateTexture2D() => new D3D11Texture2D(this);

    public ITexture2DArray CreateTexture2DArray()
    {
        throw new NotImplementedException();
    }

    public ITexture2D[] CreateTexture2Ds(int count)
    {
        throw new NotImplementedException();
    }

    public ITextureSampler CreateTextureSampler()
    {
        throw new NotImplementedException();
    }

    public IVertexArray CreateVertexArray()
    {
        throw new NotImplementedException();
    }

    public void DrawVertexArray(in IVertexArray vertexArray, PrimitiveType primitiveType, int start, uint count)
    {
        throw new NotImplementedException();
    }

    public void DrawVertexArrayInstanced(in IVertexArray vertexArray, PrimitiveType primitiveType, int start, uint count, uint instanceCount)
    {
        throw new NotImplementedException();
    }

    public void DrawVertexElementsArray(in IVertexArray vertexArray, PrimitiveType primitiveType, uint count)
    {
        throw new NotImplementedException();
    }

    public void DrawVertexElementsArrayInstanced(in IVertexArray vertexArray, PrimitiveType primitiveType, uint count, uint instanceCount)
    {
        throw new NotImplementedException();
    }

    public void UseShaderProgram(IShaderProgram? shaderProgram)
    {
        throw new NotImplementedException();
    }

    public unsafe void Viewport(Rectangle<float> rectangle, uint? index = null)
    {
        Viewport viewport = new()
        {
            TopLeftX = rectangle.Origin.X,
            TopLeftY = rectangle.Origin.Y,
            Width = rectangle.Size.X,
            Height = rectangle.Size.Y,
            MinDepth = 0f,
            MaxDepth = 1f
        };
        DeviceContext.RSSetViewports(index ?? 1, in viewport);
    }
}
