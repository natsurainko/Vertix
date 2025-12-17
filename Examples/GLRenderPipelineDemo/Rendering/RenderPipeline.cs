using GLRenderPipelineDemo.Rendering.RenderPasses;
using System.Drawing;
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

    public RenderPipeline(IGraphicsDevice d, RenderContext c, GraphicsResources graphicsResources) : base(d, c)
    {
        _graphicsResources = graphicsResources;

        AddPass(new GeometryPass(graphicsResources.GeometryPassShader, graphicsResources));
        AddPass(new AmbientOcclusionPass(graphicsResources.AmbientOcclusionPassShader, graphicsResources));
        AddPass(new DirectionalShadowPass(graphicsResources.DirectionalShadowPassShader, graphicsResources));
        AddPass(new DirectionalLightingPass(graphicsResources.DirectionalLightingPassShader, graphicsResources));

        _graphicsResources.ScreenSampleShader.Parameters["isSingleValue"].SetValue(true);
    }

    public override void FinalDraw()
    {
        _graphicsDevice.BindRenderTarget(null);
        _graphicsDevice.EnableFaceCulling = false;

        _graphicsDevice.Clear(ClearBufferMask.Color | ClearBufferMask.Depth | ClearBufferMask.Stencil, Color.CornflowerBlue);
        _graphicsDevice.UseShaderProgram(_graphicsResources.BlendPassShader);

        _graphicsResources.BlendPassShader.Parameters["viewPos"].SetValue(RenderContext.PerspectiveCamera.Position);
        _graphicsResources.BlendPassShader.Parameters["lightDirection"].SetValue(RenderContext.DirectionalLight.LightDirection);

        _graphicsDevice.BindTexture(0, RenderContext.GBufferTarget?.TargetTextures[0]);
        _graphicsDevice.BindTexture(1, RenderContext.GBufferTarget?.TargetTextures[1]);
        _graphicsDevice.BindTexture(2, RenderContext.GBufferTarget?.TargetTextures[2]);
        _graphicsDevice.BindTexture(3, RenderContext.DirectionalLightingTarget?.TargetTextures[0]);
        _graphicsDevice.BindTexture(4, RenderContext.AmbientOcclusionTarget?.TargetTextures[0]);

        //_graphicsDevice.UseShaderProgram(_graphicsResources.ScreenSampleShader);
        //_graphicsDevice.BindTexture(0, RenderContext.AmbientOcclusionTarget?.TargetTextures[0]);

        _graphicsDevice.DrawVertexArray(_graphicsResources.QuadVertexArray, PrimitiveType.TriangleStrip, 0, 4);
        _graphicsDevice.BindTexture(0, null);
    }
}
