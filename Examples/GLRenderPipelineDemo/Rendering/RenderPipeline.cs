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

        AddPass(new GeometryPass(graphicsResources.GeometryPassShader));
    }

    public override void FinalDraw()
    {
        _graphicsDevice.BindRenderTarget(null);
        _graphicsDevice.UseShaderProgram(_graphicsResources.ScreenSampleShader);

        _graphicsResources.ScreenSampleShader.Parameters["window"].SetValue(RenderContext.WindowMatrix);
        _graphicsResources.ScreenSampleShader.Parameters["view"].SetValue(RenderContext.WindowViewMatirx);
        _graphicsResources.ScreenSampleShader.Parameters["projection"].SetValue(RenderContext.WindowProjectionMatrix);

        _graphicsDevice.DrawVertexElementsArray(_graphicsResources.RectangleVertexArray, PrimitiveType.Triangles, 6);
    }
}
