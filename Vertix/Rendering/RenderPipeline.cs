using System;
using System.Collections.Generic;
using Vertix.Graphics;

namespace Vertix.Rendering;

public class RenderPipeline<TRenderContext>(IGraphicsDevice graphicsDevice, TRenderContext renderContext) : IDisposable
{
    protected readonly IGraphicsDevice _graphicsDevice = graphicsDevice;
    protected readonly List<RenderPass<TRenderContext>> _renderPasses = [];

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

    public void AddPass<TRenderPass>()
        where TRenderPass : RenderPass<TRenderContext>, new() => AddPass(new TRenderPass());

    public void AddPass<TRenderPass>(TRenderPass renderPass)
        where TRenderPass : RenderPass<TRenderContext>
    {
        renderPass.Initialize(_graphicsDevice, RenderContext);
        _renderPasses.Add(renderPass);
    }

    public void RemovePass(RenderPass<TRenderContext> pass)
    {
        if (_renderPasses.Remove(pass))
            pass.Dispose();
    }

    public void Dispose()
    {
        if (_graphicsDevice?.Disposed == true)
            return;

        for (int i = 0; i < _renderPasses.Count; i++)
        {
            RenderPass<TRenderContext> renderPass = _renderPasses[i];
            renderPass.Dispose();
        }

        _renderPasses.Clear();
    }
}
