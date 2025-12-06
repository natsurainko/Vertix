using Silk.NET.Maths;
using Vertix.Graphics;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering.RenderPasses;

internal class GeometryPass(IShaderProgram shaderProgram) : RenderPass<RenderContext>
{
    private IRenderTarget? _renderTarget;
    private ITexture2D[] _texture2Ds = [];
    private readonly IShaderProgram _shaderProgram = shaderProgram;

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
        _graphicsDevice!.BindRenderTarget(_renderTarget);
        _graphicsDevice.Clear(ClearBufferMask.Color | ClearBufferMask.Depth);
        _graphicsDevice.UseShaderProgram(_shaderProgram);

    }

    public override void Dispose()
    {
        _renderTarget?.Dispose();

        for (int i = 0; i < 4; i++)
            _texture2Ds[i].Dispose();
    }
}
