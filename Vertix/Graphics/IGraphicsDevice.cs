using Silk.NET.Maths;
using System;
using System.Drawing;
using Vertix.Graphics.Resources;
using Vertix.Rendering;

namespace Vertix.Graphics;

public interface IGraphicsDevice : IDisposable
{
    void Clear(ClearBufferMask buffers, Color color = default, float depth = 1f, int stencil = default);

    IShaderProgram CreateShaderProgram();

    void UseShaderProgram(IShaderProgram? shaderProgram);

    IGraphicsBuffer CreateGraphicsBuffer();

    IGraphicsBatcher<TInstance> CreateGraphicsBatcher<TInstance>(in IVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 128)
        where TInstance : unmanaged;

    IVertexArray CreateVertexArray();

    void InitializeModelMeshesVertexArray(in Model model);

    void DrawVertexArray(in IVertexArray vertexArray, PrimitiveType primitiveType, int start, uint count);

    void DrawVertexArrayInstanced(in IVertexArray vertexArray, PrimitiveType primitiveType,
        int start, uint count, uint instanceCount);

    void DrawVertexElementsArray(in IVertexArray vertexArray, PrimitiveType primitiveType, uint count);

    void DrawVertexElementsArrayInstanced(in IVertexArray vertexArray, PrimitiveType primitiveType,
        uint count, uint instanceCount);

    void Viewport(Vector2D<int> size);
}