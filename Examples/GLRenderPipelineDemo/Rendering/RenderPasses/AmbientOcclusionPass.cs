using Silk.NET.Maths;
using System.Numerics;
using Vertix.Graphics;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering.RenderPasses;

internal class AmbientOcclusionPass(IShaderProgram shaderProgram, GraphicsResources graphicsResources) : RenderPass<RenderContext>
{
    private IRenderTarget? _renderTarget;
    private ITexture2D? _texture2D;
    private ITexture2D? _noiseTexture2D;

    private readonly IShaderProgram _shaderProgram = shaderProgram;
    private readonly GraphicsResources _graphicsResources = graphicsResources;

    public override string Name => "AmbientOcclusionPass";

    public int KernelSize => 32;

    public override void Initialize(IGraphicsDevice graphicsDevice, RenderContext renderContext)
    {
        base.Initialize(graphicsDevice, renderContext);

        Vector2D<uint> size = renderContext.WindowRectangle.Size.As<uint>();

        _renderTarget = _graphicsDevice!.CreateRenderTarget(size);
        _texture2D = _graphicsDevice.CreateTexture2D();

        _texture2D.Initialize(size, TextureFormat.R32f);
        _renderTarget.AttachTargetTexture(_texture2D, RenderTargetAttachment.Color);

        _renderTarget.Initialize();
        _context!.AmbientOcclusionTarget = _renderTarget;

        _noiseTexture2D = GenerateNoiseTexture(4, 4);

        _shaderProgram.Parameters["samples"].SetValues(GenerateSsaoKernel(KernelSize), (uint)KernelSize);
        _shaderProgram.Parameters["kernelSize"].SetValue(KernelSize);
        _shaderProgram.Parameters["noiseScale"].SetValue(new Vector2(size.X / 4f, size.Y / 4f));
    }

    public override void Execute()
    {
        if (_graphicsDevice == null || _context == null || _renderTarget == null) return;

        _graphicsDevice.BindRenderTarget(_renderTarget);
        _graphicsDevice.Clear(ClearBufferMask.Color);

        _graphicsDevice.UseShaderProgram(_shaderProgram);
        _graphicsDevice.EnableFaceCulling = true;

        _graphicsDevice.BindTexture(0, _context.GBufferTarget?.TargetTextures[0]);
        _graphicsDevice.BindTexture(1, _context.GBufferTarget?.TargetTextures[1]);
        _graphicsDevice.BindTexture(2, _noiseTexture2D);

        _shaderProgram.Parameters["view"].SetValue(_context.CameraViewMatrix);
        _shaderProgram.Parameters["projection"].SetValue(_context.CameraProjectionMatrix);

        _graphicsDevice.DrawVertexArray(_graphicsResources.RectangleVertexArray, PrimitiveType.TriangleStrip, 0, 4);
    }

    public override void Dispose()
    {
        _renderTarget?.Dispose();
        _texture2D?.Dispose();
        _noiseTexture2D?.Dispose();
    }

    private static Vector3[] GenerateSsaoKernel(int count = 64)
    {
        List<Vector3> ssaoKernel = [];

        static float ourLerp(float a, float b, float f) => a + (f * (b - a));

        for (int i = 0; i < count; i++)
        {
            Vector3 sample = new()
            {
                X = Random.Shared.NextSingle() * 2.0f - 1.0f,
                Y = Random.Shared.NextSingle() * 2.0f - 1.0f,
                Z = Random.Shared.NextSingle()
            };

            sample = Vector3.Normalize(sample);
            sample *= Random.Shared.NextSingle();
            float scale = i / (float)count;

            // scale samples s.t. they're more aligned to center of kernel
            scale = ourLerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.Add(sample);
        }

        return [.. ssaoKernel];
    }

    private ITexture2D GenerateNoiseTexture(int w = 4, int h = 4)
    {
        int count = w * h;
        Span<Vector4> ssaoNoise = stackalloc Vector4[count];

        for (int i = 0; i < w * h; i++)
        {
            ssaoNoise[i] = new
            (
                Random.Shared.NextSingle() * 2.0f - 1.0f,
                Random.Shared.NextSingle() * 2.0f - 1.0f,
                0f,
                0f
            );
        }

        Vector2D<uint> size = new((uint)w, (uint)h);
        ITexture2D noiseTexture = _graphicsDevice!.CreateTexture2D();
        noiseTexture.Initialize(size, TextureFormat.Rgba32f);
        noiseTexture.SetData(size, Vector2D<int>.Zero, ssaoNoise);

        return noiseTexture;
    }
}
