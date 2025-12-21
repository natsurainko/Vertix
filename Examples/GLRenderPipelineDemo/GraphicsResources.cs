using Silk.NET.Maths;
using System.Numerics;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Primitives;
using Vertix.OpenGL.Extensions;
using Vertix.Rendering;
using ShaderType = Vertix.Rendering.ShaderType;

namespace GLRenderPipelineDemo;

internal class GraphicsResources : IDisposable
{
    public IVertexArray QuadVertexArray { get; private set; }

    public IGraphicsBatcher<Vertex2D.InstanceTransform2D> RectangleBatcher { get; private set; }

    public IShaderProgram ScreenSampleShader { get; private set; }

    public IShaderProgram GeometryPassShader { get; private set; }

    public IShaderProgram AmbientOcclusionPassShader { get; private set; }

    public IShaderProgram DirectionalShadowPassShader { get; private set; }

    public IShaderProgram DirectionalLightingPassShader { get; private set; }

    public IShaderProgram BlendPassShader { get; private set; }

    public ITexture2D DefaultTexture { get; private set; }

    public ITextureSampler NearestSampler { get; private set; }

    public IGraphicsBuffer LightSpaceMatircesBuffer { get; private set; }

    public GraphicsResources(IGraphicsDevice graphicsDevice)
    {
        IVertexArray vertexArray = graphicsDevice.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = graphicsDevice.CreateGraphicsBuffer(GraphicsBufferUsage.VertexBuffer);

        vertexBuffer.Initialize(Vertex2D.QuadVertices.Length, data: Vertex2D.QuadVertices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties);

        QuadVertexArray = vertexArray;
        RectangleBatcher = graphicsDevice.CreateGraphicsBatcher<Vertex2D.InstanceTransform2D>(in vertexArray,
            Vertex2D.InstanceTransform2D.DefaultProperties, (uint)Vertex2D.QuadVertices.Length);
        RectangleBatcher.PrimitiveType = PrimitiveType.TriangleStrip;

        ScreenSampleShader = graphicsDevice.CreateShaderProgram();
        ScreenSampleShader.LoadGLSLShadersFromFiles(_SCREEN_SAMPLE_SHADER);
        ScreenSampleShader.Compile();

        GeometryPassShader = graphicsDevice.CreateShaderProgram();
        GeometryPassShader.LoadGLSLShadersFromFiles(_GEOMETRY_PASS_SHADER);
        GeometryPassShader.Compile();

        AmbientOcclusionPassShader = graphicsDevice.CreateShaderProgram();
        AmbientOcclusionPassShader.LoadGLSLShadersFromFiles(_AMBIENT_OCCLUSION_PASS_SHADER);
        AmbientOcclusionPassShader.Compile();

        DirectionalShadowPassShader = graphicsDevice.CreateShaderProgram();
        DirectionalShadowPassShader.LoadGLSLShadersFromFiles(_DIRECTIONAL_SHADOW_PASS_SHADER);
        DirectionalShadowPassShader.Compile();

        DirectionalLightingPassShader = graphicsDevice.CreateShaderProgram();
        DirectionalLightingPassShader.LoadGLSLShadersFromFiles(_DIRECTIONAL_LIGHTING_PASS_SHADER);
        DirectionalLightingPassShader.Compile();

        BlendPassShader = graphicsDevice.CreateShaderProgram();
        BlendPassShader.LoadGLSLShadersFromFiles(_BLEND_PASS_SHADER);
        BlendPassShader.Compile();

        ITexture2D texture2D = graphicsDevice.CreateTexture2D();
        texture2D.Initialize(Vector2D<uint>.One, TextureFormat.Rgba8);
        texture2D.SetData(Vector2D<uint>.One, Vector2D<int>.Zero, stackalloc byte[] { 255, 255, 255, 255 });

        DefaultTexture = texture2D;

        ITextureSampler textureSampler = graphicsDevice.CreateTextureSampler();
        textureSampler.MinFilter = TextureFilter.Nearest;
        textureSampler.MagFilter = TextureFilter.Nearest;
        textureSampler.AddressU = TextureAddressMode.ClampToBorder;
        textureSampler.AddressV = TextureAddressMode.ClampToBorder;
        textureSampler.BorderColor = Vector4.One;

        NearestSampler = textureSampler;

        LightSpaceMatircesBuffer = graphicsDevice.CreateGraphicsBuffer(GraphicsBufferUsage.UniformBuffer);
        LightSpaceMatircesBuffer.Initialize<Matrix4X4<float>>(8, true);
        LightSpaceMatircesBuffer.BindAsUniform(0);
    }

    public void Dispose()
    {

    }

    internal static readonly (ShaderType, string)[] _SCREEN_SAMPLE_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/screen_sample.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/screen_sample.frag"),
    ];

    internal static readonly (ShaderType, string)[] _GEOMETRY_PASS_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/Passes/geometry_pass.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/Passes/geometry_pass.frag"),
    ];

    internal static readonly (ShaderType, string)[] _AMBIENT_OCCLUSION_PASS_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/screen_sample.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/Passes/ambient_occlusion_pass.frag"),
    ];

    internal static readonly (ShaderType, string)[] _DIRECTIONAL_SHADOW_PASS_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/Passes/directional_shadow_pass.vert"),
        (ShaderType.GeometryShader, "Assets/Shaders/Passes/directional_shadow_pass.geom"),
        (ShaderType.FragmentShader, "Assets/Shaders/Passes/directional_shadow_pass.frag"),
    ];

    internal static readonly (ShaderType, string)[] _DIRECTIONAL_LIGHTING_PASS_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/screen_sample.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/Passes/directional_lighting_pass.frag"),
    ];

    internal static readonly (ShaderType, string)[] _BLEND_PASS_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/screen_sample.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/Passes/blend_pass.frag"),
    ];
}
