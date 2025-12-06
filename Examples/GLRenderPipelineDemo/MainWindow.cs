using GLRenderPipelineDemo.Rendering;
using Silk.NET.Windowing;
using Vertix.Graphics;
using Vertix.OpenGL.Windowing;

namespace GLRenderPipelineDemo;

internal class MainWindow(IWindow w, IGraphicsDevice d, 
    RenderContext renderContext, RenderPipeline renderPipeline) : GLGameWindow(w, d)
{
    protected override void OnLoaded()
    {
        renderContext.PerspectiveCamera = new();
    }

    protected override void OnRender(double delateTime)
    {
        renderPipeline.Execute();
        renderPipeline.FinalDraw();
    }

    protected override void OnUpdate(double deltaTime)
    {
        renderContext.Update();
    }
}
