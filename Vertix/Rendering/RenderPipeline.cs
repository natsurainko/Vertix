using System;
using System.Collections.Generic;
using Vertix.Graphics;

namespace Vertix.Rendering;

public class RenderPipeline<TRenderContext>(IGraphicsDevice graphicsDevice, TRenderContext renderContext) : IDisposable
{
    private readonly IGraphicsDevice _graphicsDevice = graphicsDevice;
    private readonly List<RenderPass<TRenderContext>> _renderPasses = [];

    public TRenderContext RenderContext { get; private set; } = renderContext;

    public virtual void Execute()
    {
        for (int i = 0; i < _renderPasses.Count; i++)
        {
            RenderPass<TRenderContext> renderPass = _renderPasses[i];
            renderPass.Execute();
        }
    }

    public virtual void FinalDraw() { }

    public void AddPass<TRenderPass>(TRenderPass pass)
        where TRenderPass : RenderPass<TRenderContext>
    {
        _renderPasses.Add(pass);
        pass.Initialize(_graphicsDevice, RenderContext);
    }

    public void RemovePass(RenderPass<TRenderContext> pass)
    {
        if (_renderPasses.Remove(pass))
            pass.Dispose();
    }

    public void Dispose()
    {
        for (int i = 0; i < _renderPasses.Count; i++)
        {
            RenderPass<TRenderContext> renderPass = _renderPasses[i];
            renderPass.Dispose();
        }

        _renderPasses.Clear();
    }
}
