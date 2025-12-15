using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using System.Collections.Generic;
using System.Numerics;
using Vertix.Graphics;
using Vertix.OpenGL.Graphics;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace Vertix.OpenGL.Rendering;

public class GLRenderTarget : IRenderTarget
{
    private readonly GL _gL;
    private readonly List<ITexture> _targetTextures = [];
    private readonly bool[] _colorAttachments = new bool[32];
    private ColorBuffer[] _colorBuffers = [];

    public Vector2D<uint> Size { get; }

    public IReadOnlyList<ITexture> TargetTextures => _targetTextures;

    public uint FrameBufferHandle { get; protected set; }

    public bool Initialized { get; protected set; }

    public GLRenderTarget(GL gL, Vector2D<uint> size)
    {
        _gL = gL;
        FrameBufferHandle = _gL.CreateFramebuffer();
        Size = size;
    }

    public void AttachTargetTexture(ITexture texture, RenderTargetAttachment renderTargetAttachment = RenderTargetAttachment.Color, int mipmapIndex = 0)
    {
        if (texture is not GLTexture gLTexture)
            throw new InvalidOperationException();

        _targetTextures.Add(texture);
        _gL.NamedFramebufferTexture
        (
            FrameBufferHandle,
            GetFramebufferAttachment(renderTargetAttachment),
            gLTexture.Handle,
            mipmapIndex
        );
    }

    public void Initialize()
    {
        FramebufferStatus framebufferStatus = (FramebufferStatus)_gL.CheckNamedFramebufferStatus(FrameBufferHandle, FramebufferTarget.Framebuffer);
        if (framebufferStatus != FramebufferStatus.Complete)
            throw new InvalidOperationException(framebufferStatus.ToString());

        List<ColorBuffer> colorBuffers = [];
        for (int i = 0; i < _colorAttachments.Length; i++)
            if (_colorAttachments[i])
                colorBuffers.Add(ColorBuffer.ColorAttachment0 + i);

        _colorBuffers = [.. colorBuffers];
    }

    public void BindRenderTarget()
    {
        _gL.BindFramebuffer(FramebufferTarget.Framebuffer, FrameBufferHandle);
        _gL.NamedFramebufferDrawBuffers(FrameBufferHandle, _colorBuffers);
    }

    private FramebufferAttachment GetFramebufferAttachment(RenderTargetAttachment renderTargetAttachment)
    {
        FramebufferAttachment framebufferAttachment = renderTargetAttachment switch
        {
            RenderTargetAttachment.Depth => FramebufferAttachment.DepthAttachment,
            RenderTargetAttachment.Stencil => FramebufferAttachment.StencilAttachment,
            RenderTargetAttachment.DepthStencil => FramebufferAttachment.DepthStencilAttachment,
            _ => FramebufferAttachment.ColorAttachment0,
        };

        if (framebufferAttachment != FramebufferAttachment.ColorAttachment0)
            return framebufferAttachment;

        for (int i = 0; i < _colorAttachments.Length; i++)
        {
            if (!_colorAttachments[i])
            {
                _colorAttachments[i] = true;
                return FramebufferAttachment.ColorAttachment0 + i;
            }
        }

        return framebufferAttachment;
    }

    public unsafe void ClearTargetTexture(ClearBufferMask buffers, int index, Vector4 color = default, float depth = 1, int stencil = 0)
    {
        if ((buffers & ClearBufferMask.Color) == ClearBufferMask.Color)
            _gL.ClearNamedFramebuffer(FrameBufferHandle, BufferKind.Color, index, &color.X);
        if ((buffers & ClearBufferMask.Depth) == ClearBufferMask.Depth)
            _gL.ClearNamedFramebuffer(FrameBufferHandle, BufferKind.Depth, 0, in depth);
        if ((buffers & ClearBufferMask.Stencil) == ClearBufferMask.Stencil)
            _gL.ClearNamedFramebuffer(FrameBufferHandle, BufferKind.Stencil, 0, in stencil);
    }

    public void Dispose()
    {
        _gL.DeleteFramebuffer(FrameBufferHandle);
        _targetTextures.Clear();
    }
}
