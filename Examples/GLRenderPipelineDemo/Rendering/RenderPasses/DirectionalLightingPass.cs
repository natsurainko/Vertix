using Silk.NET.Maths;
using System.Numerics;
using Vertix.Graphics;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering.RenderPasses;

internal class DirectionalLightingPass(IShaderProgram shaderProgram, GraphicsResources graphicsResources) : RenderPass<RenderContext>
{
    private IRenderTarget? _renderTarget;
    private ITexture2D? _texture2D;

    private readonly IShaderProgram _shaderProgram = shaderProgram;
    private readonly GraphicsResources _graphicsResources = graphicsResources;

    public override string Name => "DirectionalLightingPass";

    public override void Initialize(IGraphicsDevice graphicsDevice, RenderContext renderContext)
    {
        base.Initialize(graphicsDevice, renderContext);

        Vector2D<uint> size = renderContext.WindowRectangle.Size.As<uint>();

        _renderTarget = _graphicsDevice!.CreateRenderTarget(size);
        _texture2D = _graphicsDevice.CreateTexture2D();

        _texture2D.Initialize(size, TextureFormat.R16f);
        _renderTarget.AttachTargetTexture(_texture2D, RenderTargetAttachment.Color);

        _renderTarget.Initialize();
        _context!.DirectionalLightingTarget = _renderTarget;

        _shaderProgram.Parameters["cascadeCount"].SetValue(4);
        _shaderProgram.Parameters["cascadePlaneDistances"].SetValues(_context.CascadePlaneDistances, 4);
        _shaderProgram.Parameters["shadowMapTexelSize"].SetValue(new Vector2(1f / _context.ShadowMapSize.X, 1f / _context.ShadowMapSize.Y));
    }

    public override void Execute()
    {
        if (_graphicsDevice == null || _context == null || _renderTarget == null) return;

        _graphicsDevice.BindRenderTarget(_renderTarget);
        _graphicsDevice.Clear(ClearBufferMask.Color);

        _graphicsDevice.UseShaderProgram(_shaderProgram);
        _graphicsDevice.EnableFaceCulling = false;

        _shaderProgram.Parameters["lightDirection"].SetValue(_context.DirectionalLight.LightDirection);

        _graphicsDevice.BindTexture(0, _context.GBufferTarget?.TargetTextures[0]);
        _graphicsDevice.BindTexture(1, _context.GBufferTarget?.TargetTextures[1]);
        _graphicsDevice.BindTexture(2, _context.DirectionalShadowTarget?.TargetTextures[0]);
        _graphicsDevice.BindTextureSampler(2, _graphicsResources.NearestSampler);

        _graphicsDevice.DrawVertexArray(_graphicsResources.QuadVertexArray, PrimitiveType.TriangleStrip, 0, 4);
        _graphicsDevice.BindTextureSampler(2, null);
    }

    public override void Dispose()
    {
        _renderTarget?.Dispose();
        _texture2D?.Dispose();
    }
}
