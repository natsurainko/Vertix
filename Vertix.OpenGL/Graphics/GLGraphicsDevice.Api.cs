using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using System.Drawing;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;
using Vertix.OpenGL.Rendering;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;
using PrimitiveType = Vertix.Graphics.PrimitiveType;
using ShaderType = Vertix.Rendering.ShaderType;

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

        if (renderTarget is not GLRenderTarget gLRenderTarget)
            throw new InvalidOperationException();

        GL.BindFramebuffer(FramebufferTarget.Framebuffer, gLRenderTarget.FrameBufferHandle);
        GL.NamedFramebufferDrawBuffers(gLRenderTarget.FrameBufferHandle, gLRenderTarget._colorBuffers);

        _currentRenderTarget = renderTarget;
    }

    public void BindTexture(uint bindingIndex, ITexture? texture, ShaderType? _ = null)
    {
        if (texture == null)
        {
            GL.BindTextureUnit(bindingIndex, 0);
            return;
        }

        if (texture is not GLTexture gLTexture)
            throw new InvalidOperationException();

        GL.BindTextureUnit(bindingIndex, gLTexture.Handle);
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

    public IGraphicsBuffer CreateGraphicsBuffer(GraphicsBufferUsage bufferUsage, GraphicsBufferMapAccess bufferMapAccess = GraphicsBufferMapAccess.None) => new GLGraphicsBuffer(GL, bufferUsage, bufferMapAccess);

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

    public void Viewport(Rectangle<float> rectangle, uint? index = null) => GL.ViewportIndexed(index ?? 0, rectangle.Origin.X, rectangle.Origin.Y, rectangle.Size.X, rectangle.Size.Y);
}
