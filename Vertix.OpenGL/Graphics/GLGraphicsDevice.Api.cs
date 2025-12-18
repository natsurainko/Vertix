using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using System.Drawing;
using Vertix.Graphics;
using Vertix.Graphics.Primitives;
using Vertix.OpenGL.Helpers;
using Vertix.OpenGL.Rendering;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;
using PrimitiveType = Vertix.Graphics.PrimitiveType;

namespace Vertix.OpenGL.Graphics;

public partial class GLGraphicsDevice : IGraphicsDevice
{
    public bool EnableDepthTest
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                if (value)
                    GL.Enable(EnableCap.DepthTest);
                else GL.Disable(EnableCap.DepthTest);
            }
        }
    }

    public bool EnableFaceCulling
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                if (value)
                    GL.Enable(EnableCap.CullFace);
                else GL.Disable(EnableCap.CullFace);
            }
        }
    }

    public CullFaceMode CullFace
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                GL.CullFace(value switch
                {
                    CullFaceMode.FrontFace => TriangleFace.Front,
                    CullFaceMode.BackAndFrontFace => TriangleFace.FrontAndBack,
                    _ => TriangleFace.Back,
                });
            }
        }
    }

    public FaceWindingOrder FaceWindingOrder
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                GL.FrontFace(value switch
                {
                    FaceWindingOrder.Clockwise => FrontFaceDirection.CW,
                    _ => FrontFaceDirection.Ccw,
                });
            }
        }
    }

    public void BindRenderTarget(IRenderTarget? renderTarget)
    {
        if (renderTarget == _currentRenderTarget) return;
        if (renderTarget == null)
        {
            GL.BindFramebuffer(FramebufferTarget.Framebuffer, 0);
            _currentRenderTarget = null;
            return;
        }

        renderTarget.BindRenderTarget();
        _currentRenderTarget = renderTarget;
    }

    public void BindTexture(uint bindingIndex, ITexture? texture)
    {
        if (texture == null)
        {
            GL.BindTextureUnit(bindingIndex, 0);
            return;
        }

        if (texture is not GLTexture gLTexture)
            throw new InvalidOperationException();

        gLTexture.BindTexture(bindingIndex);
    }

    public void BindTextureSampler(uint bindingIndex, ITextureSampler? textureSampler)
    {
        if (textureSampler == null)
        {
            GL.BindSampler(bindingIndex, 0);
            return;
        }

        if (textureSampler is not GLTextureSampler gLTextureSampler)
            throw new InvalidOperationException();

        gLTextureSampler.BindSampler(bindingIndex);
    }

    public unsafe void Clear(ClearBufferMask buffers, Color color = default, float depth = 1f, int stencil = 0)
    {
        Silk.NET.OpenGL.ClearBufferMask glClearMask = 0;

        if ((buffers & ClearBufferMask.Color) == ClearBufferMask.Color)
        {
            GL.ClearColor(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
            glClearMask |= Silk.NET.OpenGL.ClearBufferMask.ColorBufferBit;
        }

        if ((buffers & ClearBufferMask.Depth) == ClearBufferMask.Depth)
        {
            GL.ClearDepth(depth);
            glClearMask |= Silk.NET.OpenGL.ClearBufferMask.DepthBufferBit;
        }

        if ((buffers & ClearBufferMask.Stencil) == ClearBufferMask.Stencil)
        {
            GL.ClearStencil(stencil);
            glClearMask |= Silk.NET.OpenGL.ClearBufferMask.StencilBufferBit;
        }

        if (glClearMask != 0)
            GL.Clear(glClearMask);
    }

    public IShaderProgram CreateShaderProgram() => new GLShaderProgram(GL);

    public IVertexArray CreateVertexArray() => new GLVertexArray(GL);

    public IGraphicsBuffer CreateGraphicsBuffer() => new GLGraphicsBuffer(GL);

    public IRenderTarget CreateRenderTarget(Vector2D<uint> size) => new GLRenderTarget(GL, size);

    public ITexture2D CreateTexture2D() => new GLTexture2D(GL);

    public ITexture2D[] CreateTexture2Ds(int count)
    {
        Span<uint> handles = stackalloc uint[count];
        GL.CreateTextures(TextureTarget.Texture2D, handles);
        ITexture2D[] texture2Ds = new ITexture2D[count];

        for (int i = 0; i < count; i++)
            texture2Ds[i] = new GLTexture2D(GL, handles[i]);

        return texture2Ds;
    }

    public ITexture2DArray CreateTexture2DArray() => new GLTexture2DArray(GL);

    public ITextureSampler CreateTextureSampler() => new GLTextureSampler(GL);

    public IGraphicsBatcher<TInstance> CreateGraphicsBatcher<TInstance>(in IVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 4096)
        where TInstance : unmanaged => new GLGraphicsBatcher<TInstance>(this, vertexArray, properties, verticesOrIndicesCount, capacity);

    //public unsafe void InitializeModelMeshesVertexArray(in Model model)
    //{
    //    int meshLength = model.Meshes.Length;
    //    Span<uint> vertexArrays = stackalloc uint[meshLength];
    //    Span<uint> buffers = stackalloc uint[meshLength * 2];
    //    GL.CreateVertexArrays((uint)meshLength, vertexArrays);
    //    GL.CreateBuffers((uint)(meshLength * 2), buffers);

    //    for (int i = 0; i < model.Meshes.Length; i++)
    //    {
    //        Mesh mesh = model.Meshes[i];

    //        GLGraphicsBuffer vertexBuffer = new(GL, buffers[i * 2]);
    //        GLGraphicsBuffer indexBuffer = new(GL, buffers[(i * 2) + 1]);
    //        GLVertexArray vertexArray = new(GL, vertexArrays[i]);

    //        vertexBuffer.Initialize(mesh.Vertices.Length, (uint)BufferStorageMask.None, mesh.Vertices);
    //        indexBuffer.Initialize(mesh.Indices.Length, (uint)BufferStorageMask.None, mesh.Indices);
    //        vertexArray.Initialize<Vertex>(vertexBuffer, Vertex.DefaultProperties, indexBuffer);

    //        model.Meshes[i].VertexArray = vertexArray;
    //    }
    //}

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

    public void UseShaderProgram(IShaderProgram? shaderProgram)
    {
        if (shaderProgram == _currentShaderProgram) return;
        if (shaderProgram == null)
        {
            GL.UseProgram(0);
            _currentShaderProgram = null;
            return;
        }

        if (shaderProgram is not GLShaderProgram gLShaderProgram)
            throw new InvalidOperationException();

        gLShaderProgram.Use();
        _currentShaderProgram = shaderProgram;
    }

    public void Viewport(Vector2D<int> size) => GL.Viewport(size);
}
