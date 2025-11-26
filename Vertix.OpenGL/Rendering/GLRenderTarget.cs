using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using System.Collections.Generic;
using Vertix.Graphics;
using Vertix.OpenGL.Graphics;
using Vertix.Rendering;

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

    public void AttachTargetTexture(ITexture texture, RenderTargetAttachment renderTargetAttachment = RenderTargetAttachment.Color)
    {
        if (texture is not GLTexture gLTexture) 
            throw new InvalidOperationException();

        _targetTextures.Add(texture);
        _gL.NamedFramebufferTexture
        (
            FrameBufferHandle,
            GetFramebufferAttachment(renderTargetAttachment),
            gLTexture.Handle, 
            (int)gLTexture.MipmapLevels
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

    public void BindRenderTarget() => _gL.NamedFramebufferDrawBuffers(FrameBufferHandle, _colorBuffers);

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

    public void Dispose()
    {
        _gL.DeleteFramebuffer(FrameBufferHandle);
        _targetTextures.Clear();
    }
}
