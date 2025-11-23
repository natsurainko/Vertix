using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using SkiaSharp;
using System.Drawing;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Graphics;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLGameDemo.Windows;

internal class Texture2DTestWindow(IWindow w) : GLGameWindow(w)
{
    static readonly (ShaderType, string)[] _gLSLSources =
    [
        (ShaderType.VertexShader, "Assets/Shaders/2D/rectangle.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/2D/rectangle.frag"),
    ];

    static readonly uint[] _indices = [0, 1, 3, 1, 2, 3];
    static readonly Vertex2D[] _vertices =
    [
        new() { Position = new(-1, -1), ZIndex = 0, Color = Color.Red.ToVector4() },
        new() { Position = new(-1, 1), ZIndex = 0, Color = Color.Red.ToVector4() },
        new() { Position = new(1, 1), ZIndex = 0, Color = Color.Red.ToVector4() },
        new() { Position = new(1, -1), ZIndex = 0, Color = Color.Red.ToVector4() },
    ];

    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Texture2D Test Window";

        _gL.Enable(EnableCap.DepthTest);
        _gL.Enable(EnableCap.Blend);
        _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

        IVertexArray vertexArray = Graphics.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

        ITexture2D texture = Graphics.CreateTexture2D();

        vertexBuffer.Initialize(_vertices.Length, (uint)BufferStorageMask.None, _vertices);
        indexBuffer.Initialize(_indices.Length, (uint)BufferStorageMask.None, _indices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

        using (var bitmap = SKBitmap.Decode("Assets/Shed..png")) 
        {
            Vector2D<uint> size = new((uint)bitmap.Width, (uint)bitmap.Height);

            texture.Initialize(size, (uint)InternalFormat.Rgba8);
            texture.SetData(size, Vector2D<int>.Zero, (uint)PixelFormat.Bgra, (uint)PixelType.UnsignedByte, bitmap.GetPixelSpan());
            ((GLTexture2D)texture).BindTexture(0);
        }

        ITextureSampler textureSampler = Graphics.CreateTexture2DSampler(texture);
        textureSampler.MinFilter = TextureFilter.Nearest;
        textureSampler.MagFilter = TextureFilter.Nearest;
        textureSampler.AddressU = TextureAddressMode.ClampToEdge;
        textureSampler.AddressV = TextureAddressMode.ClampToEdge;
        textureSampler.AddressW = TextureAddressMode.ClampToEdge;

        graphicsBatcher = Graphics.CreateGraphicsBatcher<Vertex2D.InstanceTransform2D>(in vertexArray, Vertex2D.InstanceTransform2D.DefaultProperties, (uint)_indices.Length);

        shader = Graphics.CreateShaderProgram();
        shader.LoadGLSLShadersFromFiles(_gLSLSources);
        shader.Compile();

        Graphics.UseShaderProgram(shader);

        shader.Parameters["screenSizeInv"].SetValue(new Vector2D<float>(1f / CoreWindow.Size.X, 1f / CoreWindow.Size.Y));
        shader.Parameters["isInstance"].SetValue(true);

        CoreWindow.Resize += CoreWindow_Resize;
    }

    private void CoreWindow_Resize(Vector2D<int> size)
    {
        Graphics.Viewport(size);
        shader?.Parameters["screenSizeInv"].SetValue(new Vector2D<float>(1f / CoreWindow.Size.X, 1f / CoreWindow.Size.Y));
    }

    protected unsafe override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.CornflowerBlue);

        graphicsBatcher?.DrawInstance(new Vertex2D.InstanceTransform2D()
        {
            Color = Color.White.ToVector4(),
            ZIndex = 0,
            Position = new Vector2D<float>(100, 100),
            Size = new Vector2D<float>(224, 256),
        });

        graphicsBatcher?.Flush();
    }

    protected override void OnUpdate(double deltaTime) { }
}
