using Silk.NET.Maths;
using System;
using System.Drawing;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.Rendering;

namespace Vertix.Direct3D12.Windowing;

public partial class D3D12GraphicsDevice : IGraphicsDevice
{
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

    public IShaderProgram CreateShaderProgram()
    {
        throw new NotImplementedException();
    }

    public ITexture2D CreateTexture2D()
    {
        throw new NotImplementedException();
    }

    public ITextureSampler CreateTexture2DSampler(ITexture2D texture2D)
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

    public void InitializeModelMeshesVertexArray(in Model model)
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