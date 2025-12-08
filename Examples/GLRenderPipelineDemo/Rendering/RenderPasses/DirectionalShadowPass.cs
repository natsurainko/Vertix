using Silk.NET.Maths;
using Vertix.Engine;
using Vertix.Graphics;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering.RenderPasses;

internal class DirectionalShadowPass(IShaderProgram shaderProgram) : RenderPass<RenderContext>
{
    private IRenderTarget? _renderTarget;
    private ITexture2D? _texture2D;

    private readonly IShaderProgram _shaderProgram = shaderProgram;

    public Vector2D<int> ShadowMapSize { get; init; } = new(2048, 2048);

    public override string Name => "DirectionalShadowPass";

    public override void Initialize(IGraphicsDevice graphicsDevice, RenderContext renderContext)
    {
        base.Initialize(graphicsDevice, renderContext);

        _renderTarget = _graphicsDevice!.CreateRenderTarget(ShadowMapSize.As<uint>());
        _texture2D = _graphicsDevice.CreateTexture2D();

        // Depth Texture
        _texture2D.Initialize(ShadowMapSize.As<uint>(), TextureFormat.Depth32f);
        _renderTarget.AttachTargetTexture(_texture2D, RenderTargetAttachment.Depth);

        _renderTarget.Initialize();
        _context!.DirectionalShadowTarget = _renderTarget;
    }

    public override void Execute()
    {
        if (_graphicsDevice == null || _context == null || _renderTarget == null) return;

        _renderTarget.Clear(ClearBufferMask.Depth);
        _graphicsDevice.BindRenderTarget(_renderTarget);
        _graphicsDevice.Viewport(ShadowMapSize);

        _graphicsDevice.UseShaderProgram(_shaderProgram);
        _shaderProgram.Parameters["lightSpaceMatrix"].SetValue(_context.DirectionalLight.LightViewProjectionMatrix);

        for (int i = 0; i < _context.SceneManager.SceneObjects.Count; i++)
        {
            GameObject3D gameObject3D = _context.SceneManager.SceneObjects[i];

            _shaderProgram.Parameters["world"].SetValue(gameObject3D.WorldMatrix);
            gameObject3D.Draw(_graphicsDevice);
        }

        _graphicsDevice.Viewport(_context.WindowRectangle.Size);
    }

    public override void Dispose()
    {
        _renderTarget?.Dispose();
        _texture2D?.Dispose();
    }
}
