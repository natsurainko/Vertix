using System;
using System.Drawing;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.Rendering;

namespace Vertix.Direct3D12.Windowing;

public partial class D3D12GraphicsDevice : IGraphicsDevice
{
    public unsafe void Clear(ClearBufferMask buffers, 
        Color color = default, float depth = 0, int stencil = 0)
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

    public IVertexArray CreateVertexArray()
    {
        throw new NotImplementedException();
    }

    public void DrawVertexElementsArray(in IVertexArray vertexArray, PrimitiveType primitiveType, uint count)
    {
        throw new NotImplementedException();
    }

    public void InitializeGraphicsBuffer<T>(in IGraphicsBuffer buffer, int length, uint flags, T[]? data = null) where T : unmanaged
    {
        throw new NotImplementedException();
    }

    public void InitializeModelMeshesVertexArray(in Model model)
    {
        throw new NotImplementedException();
    }

    public void InitializeVertexArray<T>(ref IVertexArray vertexArray, IGraphicsBuffer vertexBuffer, IVertexArrayProperty[] properties, IGraphicsBuffer? indexBuffer = null) where T : unmanaged
    {
        throw new NotImplementedException();
    }

    public void UseShaderProgram(IShaderProgram? shaderProgram)
    {
        throw new NotImplementedException();
    }
}