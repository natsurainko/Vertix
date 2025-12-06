using GLRenderPipelineDemo.Rendering.RenderPasses;
using Vertix.Graphics;

namespace GLRenderPipelineDemo.Rendering;

// Geometry Pass
//    ↓
//    ├─→ SSAO Pass → AO Texture
//    └─→ Shadow Pass → Shadow Maps
//            ↓
//        Lighting Pass(Need GBuffer + AO + Shadows)
//            ↓
//        Post-Processing

internal class RenderPipeline : Vertix.Rendering.RenderPipeline<RenderContext>
{
    private readonly GraphicsResources _graphicsResources;

    public RenderPipeline(IGraphicsDevice d, RenderContext c, GraphicsResources graphicsResources) : base(d,c)
    {
        _graphicsResources = graphicsResources;

        AddPass(new GeometryPass(graphicsResources.GeometryPassShader, graphicsResources));
    }

    public override void FinalDraw()
    {
        _graphicsDevice.Clear(ClearBufferMask.Color | ClearBufferMask.Depth | ClearBufferMask.Stencil);
        _graphicsDevice.UseShaderProgram(_graphicsResources.ScreenSampleShader);
        _graphicsDevice.BindTexture(0, RenderContext.GBufferTarget?.TargetTextures[0]);

        _graphicsDevice.DrawVertexArray(_graphicsResources.RectangleVertexArray, PrimitiveType.TriangleStrip, 0, 4);
        _graphicsDevice.BindTexture(0, null);
    }
}
