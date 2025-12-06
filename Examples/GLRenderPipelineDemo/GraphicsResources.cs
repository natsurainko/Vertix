using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Extensions;
using Vertix.Rendering;

namespace GLRenderPipelineDemo;

internal class GraphicsResources : IDisposable
{
    public IVertexArray RectangleVertexArray { get; private set; }

    public IGraphicsBatcher<Vertex2D.InstanceTransform2D> RectangleBatcher { get; private set; }

    public IShaderProgram Basic2DShader { get; private set; }

    public IShaderProgram Basic3DShader { get; private set; }

    public IShaderProgram ScreenSampleShader { get; private set; }

    public IShaderProgram GeometryPassShader { get; private set; }

    public ITexture2D DefaultTexture { get; private set; }

    public GraphicsResources(IGraphicsDevice graphicsDevice)
    {
        IVertexArray vertexArray = graphicsDevice.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = graphicsDevice.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = graphicsDevice.CreateGraphicsBuffer();

        vertexBuffer.Initialize(RectangleVertices.Length, (uint)BufferStorageMask.None, RectangleVertices);
        //indexBuffer.Initialize(RectangleIndices.Length, (uint)BufferStorageMask.None, RectangleIndices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

        RectangleVertexArray = vertexArray;
        RectangleBatcher = graphicsDevice.CreateGraphicsBatcher<Vertex2D.InstanceTransform2D>(in vertexArray,
            Vertex2D.InstanceTransform2D.DefaultProperties, (uint)RectangleVertices.Length);
        RectangleBatcher.PrimitiveType = Vertix.Graphics.PrimitiveType.TriangleStrip;

        Basic2DShader = graphicsDevice.CreateShaderProgram();
        Basic2DShader.LoadGLSLShadersFromFiles(_2D_BASIC_SHADER);
        Basic2DShader.Compile();

        Basic3DShader = graphicsDevice.CreateShaderProgram();
        Basic3DShader.LoadGLSLShadersFromFiles(_3D_BASIC_SHADER);
        Basic3DShader.Compile();

        ScreenSampleShader = graphicsDevice.CreateShaderProgram();
        ScreenSampleShader.LoadGLSLShadersFromFiles(_SCREEN_SAMPLE_SHADER);
        ScreenSampleShader.Compile();

        GeometryPassShader = graphicsDevice.CreateShaderProgram();
        GeometryPassShader.LoadGLSLShadersFromFiles(_GEOMETRY_PASS_SHADER);
        GeometryPassShader.Compile();

        ITexture2D texture2D = graphicsDevice.CreateTexture2D();
        texture2D.Initialize(Vector2D<uint>.One, TextureFormat.Rgba8);
        texture2D.SetData(Vector2D<uint>.One, Vector2D<int>.Zero, stackalloc byte[] { 255, 0, 0, 255 });

        DefaultTexture = texture2D;
    }

    public void Dispose()
    {

    }

    internal static readonly (ShaderType, string)[] _3D_BASIC_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/shader3D.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/shader3D.frag"),
    ];

    internal static readonly (ShaderType, string)[] _2D_BASIC_SHADER =
    [
        (ShaderType.VertexShader, "Assets/Shaders/shader2D.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/shader2D.frag"),
    ];

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

    internal static readonly Vertex2D[] RectangleVertices =
    [
        new() { Position = new(-1, 1, 0), TextureCoord = new(0, 1) },
        new() { Position = new(-1, -1, 0), TextureCoord = new(0, 0) },
        new() { Position = new(1, 1, 0), TextureCoord = new(1, 1) },
        new() { Position = new(1, -1, 0), TextureCoord = new(1, 0) },
    ];
}
