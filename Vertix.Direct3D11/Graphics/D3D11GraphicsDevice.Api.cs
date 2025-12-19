using Silk.NET.Maths;
using System;
using System.Drawing;
using Vertix.Graphics;
using Vertix.Rendering;

namespace Vertix.Direct3D11.Graphics;

public partial class D3D11GraphicsDevice : IGraphicsDevice
{
    public bool EnableDepthTest { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
    public bool EnableFaceCulling { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
    public CullFaceMode CullFace { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
    public FaceWindingOrder FaceWindingOrder { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public bool Disposed => throw new NotImplementedException();

    public void BindRenderTarget(IRenderTarget? renderTarget)
    {
        throw new NotImplementedException();
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
        throw new NotImplementedException();
    }

    public IGraphicsBatcher<TInstance> CreateGraphicsBatcher<TInstance>(in IVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 128) where TInstance : unmanaged
    {
        throw new NotImplementedException();
    }

    public IGraphicsBuffer CreateGraphicsBuffer()
    {
        throw new NotImplementedException();
    }

    public IRenderTarget CreateRenderTarget(Vector2D<uint> size)
    {
        throw new NotImplementedException();
    }

    public IShaderProgram CreateShaderProgram()
    {
        throw new NotImplementedException();
    }

    public ITexture2D CreateTexture2D()
    {
        throw new NotImplementedException();
    }

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

    public void Viewport(Vector2D<int> size)
    {
        throw new NotImplementedException();
    }
}
