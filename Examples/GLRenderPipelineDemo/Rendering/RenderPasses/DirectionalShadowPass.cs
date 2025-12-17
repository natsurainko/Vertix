using Silk.NET.Maths;
using Vertix.Engine;
using Vertix.Graphics;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering.RenderPasses;

internal class DirectionalShadowPass(IShaderProgram shaderProgram, GraphicsResources graphicsResources) : RenderPass<RenderContext>
{
    private IRenderTarget? _renderTarget;
    private ITexture2DArray? _texture2DArray;

    private readonly IShaderProgram _shaderProgram = shaderProgram;
    private readonly GraphicsResources _graphicsResources = graphicsResources;

    public override string Name => "DirectionalShadowPass";

    public override void Initialize(IGraphicsDevice graphicsDevice, RenderContext renderContext)
    {
        base.Initialize(graphicsDevice, renderContext);

        Vector2D<uint> size = _context!.ShadowMapSize.As<uint>();

        _renderTarget = _graphicsDevice!.CreateRenderTarget(size);
        _texture2DArray = _graphicsDevice.CreateTexture2DArray();

        // Depth Texture
        _texture2DArray.Initialize(size, (uint)renderContext.LightSpaceMatrices.Length, TextureFormat.Depth32f);
        _renderTarget.AttachTargetTexture(_texture2DArray, RenderTargetAttachment.Depth);

        _renderTarget.Initialize();
        _context!.DirectionalShadowTarget = _renderTarget;
    }

    public override void Execute()
    {
        if (_graphicsDevice == null || _context == null || _renderTarget == null) return;

        _graphicsDevice.BindRenderTarget(_renderTarget);
        _graphicsDevice.Clear(ClearBufferMask.Depth);

        _graphicsDevice.Viewport(_context.ShadowMapSize);
        _graphicsDevice.UseShaderProgram(_shaderProgram);
        _graphicsDevice.EnableFaceCulling = false;

        _graphicsResources.LightSpaceMatircesBuffer.Fill(0, 5, _context.LightSpaceMatrices);

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
        _texture2DArray?.Dispose();
    }
}
