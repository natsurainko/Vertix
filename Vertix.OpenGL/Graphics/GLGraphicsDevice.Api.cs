using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using System.Drawing;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Helpers;
using Vertix.OpenGL.Rendering;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;
using PrimitiveType = Vertix.Graphics.PrimitiveType;

namespace Vertix.OpenGL.Graphics;

public partial class GLGraphicsDevice : IGraphicsDevice
{
    public unsafe void Clear(ClearBufferMask buffers, Color color = default, float depth = 1f, int stencil = 0)
    {
        if ((buffers & ClearBufferMask.Color) == ClearBufferMask.Color)
        {
            float* colorsPtr = stackalloc float[4];
            colorsPtr[0] = color.R / 255.0f;
            colorsPtr[1] = color.G / 255.0f;
            colorsPtr[2] = color.B / 255.0f;
            colorsPtr[3] = color.A / 255.0f;

            GL.ClearBuffer(BufferKind.Color, 0, colorsPtr);
        }
        if ((buffers & ClearBufferMask.Depth) == ClearBufferMask.Depth)
            GL.ClearBuffer(BufferKind.Depth, 0, in depth);
        if ((buffers & ClearBufferMask.Stencil) == ClearBufferMask.Stencil)
            GL.ClearBuffer(BufferKind.Stencil, 0, in stencil);
    }

    public IShaderProgram CreateShaderProgram() => new GLShaderProgram(GL);

    public IGraphicsBatcher<TInstance> CreateGraphicsBatcher<TInstance>(in IVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 4096)
        where TInstance : unmanaged => new GLGraphicsBatcher<TInstance>(this, vertexArray, properties, verticesOrIndicesCount, capacity);

    public void UseShaderProgram(IShaderProgram? shaderProgram)
    {
        if (shaderProgram == null)
        {
            GL.UseProgram(0);
            return;
        }

        if (shaderProgram is not GLShaderProgram gLShaderProgram)
            throw new InvalidOperationException();

        gLShaderProgram.Use();
    }

    public unsafe void InitializeModelMeshesVertexArray(in Model model)
    {
        int meshLength = model.Meshes.Length;
        Span<uint> vertexArrays = stackalloc uint[meshLength];
        Span<uint> buffers = stackalloc uint[meshLength * 2];
        GL.CreateVertexArrays((uint)meshLength, vertexArrays);
        GL.CreateBuffers((uint)(meshLength * 2), buffers);

        for (int i = 0; i < model.Meshes.Length; i++)
        {
            Mesh mesh = model.Meshes[i];

            GLGraphicsBuffer vertexBuffer = new(GL, buffers[i * 2]);
            GLGraphicsBuffer indexBuffer = new(GL, buffers[(i * 2) + 1]);
            GLVertexArray vertexArray = new(GL, vertexArrays[i]);

            vertexBuffer.Initialize(mesh.Vertices.Length, (uint)BufferStorageMask.None, mesh.Vertices);
            indexBuffer.Initialize(mesh.Indices.Length, (uint)BufferStorageMask.None, mesh.Indices);
            vertexArray.Initialize<Vertex>(vertexBuffer, Vertex.DefaultProperties, indexBuffer);

            model.Meshes[i].VertexArray = vertexArray;
        }
    }

    public IGraphicsBuffer CreateGraphicsBuffer() => new GLGraphicsBuffer(GL);

    public IVertexArray CreateVertexArray() => new GLVertexArray(GL);

    public void DrawVertexArray(in IVertexArray vertexArray, PrimitiveType primitiveType, int start, uint count)
    {
        vertexArray.Bind();
        GL.DrawArrays(EnumHelper.ToGLEnum(primitiveType), start, count);
        GL.BindVertexArray(0);
    }

    public unsafe void DrawVertexArrayInstanced(in IVertexArray vertexArray, PrimitiveType primitiveType, int start, uint count, uint instanceCount)
    {
        vertexArray.Bind();
        GL.DrawArraysInstanced(EnumHelper.ToGLEnum(primitiveType), start, count, instanceCount);
        GL.BindVertexArray(0);
    }

    public unsafe void DrawVertexElementsArray(in IVertexArray vertexArray, PrimitiveType primitiveType, uint count)
    {
        vertexArray.Bind();
        GL.DrawElements(EnumHelper.ToGLEnum(primitiveType), count, DrawElementsType.UnsignedInt, (void*)0);
        GL.BindVertexArray(0);
    }

    public unsafe void DrawVertexElementsArrayInstanced(in IVertexArray vertexArray, PrimitiveType primitiveType, uint count, uint instanceCount)
    {
        vertexArray.Bind();
        GL.DrawElementsInstanced(EnumHelper.ToGLEnum(primitiveType), count, DrawElementsType.UnsignedInt, (void*)0, instanceCount);
        GL.BindVertexArray(0);
    }

    public void Viewport(Vector2D<int> size) => GL.Viewport(size);
}
