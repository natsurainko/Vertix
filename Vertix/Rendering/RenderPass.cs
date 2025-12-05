using System;
using Vertix.Graphics;

namespace Vertix.Rendering;

public abstract class RenderPass<TRenderContext> : IDisposable
{
    protected IGraphicsDevice? _graphicsDevice;
    protected TRenderContext? _context;

    public abstract string Name { get; }

    public bool Enabled { get; set; } = true;

    public virtual void Initialize(IGraphicsDevice graphicsDevice, TRenderContext renderContext)
    {
        _graphicsDevice = graphicsDevice;
        _context = renderContext;
    }

    public abstract void Execute();

    public virtual void Dispose() { }
}
