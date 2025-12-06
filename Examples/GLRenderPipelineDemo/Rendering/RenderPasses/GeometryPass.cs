using Silk.NET.Maths;
using Vertix.Engine;
using Vertix.Graphics;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLRenderPipelineDemo.Rendering.RenderPasses;

internal class GeometryPass(IShaderProgram shaderProgram, GraphicsResources graphicsResources) : RenderPass<RenderContext>
{
    private IRenderTarget? _renderTarget;
    private ITexture2D[] _texture2Ds = [];

    private readonly IShaderProgram _shaderProgram = shaderProgram;
    private readonly GraphicsResources _graphicsResources = graphicsResources;

    public override string Name => "GeometryPass";

    public override void Initialize(IGraphicsDevice graphicsDevice, RenderContext renderContext)
    {
        base.Initialize(graphicsDevice, renderContext);

        Vector2D<uint> size = renderContext.WindowRectangle.Size.As<uint>();

        _renderTarget = _graphicsDevice!.CreateRenderTarget(size);
        _texture2Ds = _graphicsDevice.CreateTexture2Ds(4);

        // Position - RGB: World Position (xyz) - A: Unused
        _texture2Ds[0].Initialize(size, TextureFormat.Rgba16f);
        _renderTarget.AttachTargetTexture(_texture2Ds[0], RenderTargetAttachment.Color);

        // Normal + Roughness - RGB: World Normal (xyz) - A: Roughness / Shininess
        _texture2Ds[1].Initialize(size, TextureFormat.Rgba16f);
        _renderTarget.AttachTargetTexture(_texture2Ds[1], RenderTargetAttachment.Color);

        // Albedo + Metallic - RGB: Base Color (Albedo) - A: Metallic or Specular
        _texture2Ds[2].Initialize(size, TextureFormat.Rgba8);
        _renderTarget.AttachTargetTexture(_texture2Ds[2], RenderTargetAttachment.Color);

        // Depth Buffer
        _texture2Ds[3].Initialize(size, TextureFormat.Depth24Stencil8);
        _renderTarget.AttachTargetTexture(_texture2Ds[3], RenderTargetAttachment.DepthStencil);

        _renderTarget.Initialize();
        _context!.GBufferTarget = _renderTarget;
    }

    public override void Execute()
    {
        if (_graphicsDevice == null || _context == null || _renderTarget == null) return;

        _renderTarget.Clear(ClearBufferMask.Color | ClearBufferMask.Depth | ClearBufferMask.Stencil);
        _graphicsDevice.BindRenderTarget(_renderTarget);
        _graphicsDevice.UseShaderProgram(_shaderProgram);

        _graphicsDevice.BindTexture(0, _graphicsResources.DefaultTexture);
        _graphicsDevice.BindTexture(1, _graphicsResources.DefaultTexture);

        _shaderProgram.Parameters["view"].SetValue(_context.CameraViewMatrix);
        _shaderProgram.Parameters["projection"].SetValue(_context.CameraProjectionMatrix);

        for (int i = 0; i < _context.SceneManager.SceneObjects.Count; i++)
        {
            GameObject3D gameObject3D = _context.SceneManager.SceneObjects[i];

            _shaderProgram.Parameters["world"].SetValue(gameObject3D.WorldMatrix);
            gameObject3D.Draw(_graphicsDevice);
        }

        _graphicsDevice.BindTexture(0, null);
        _graphicsDevice.BindTexture(1, null);
        _graphicsDevice.BindRenderTarget(null);
    }

    public override void Dispose()
    {
        _renderTarget?.Dispose();

        for (int i = 0; i < 4; i++)
            _texture2Ds[i].Dispose();
    }
}
