using Silk.NET.Assimp;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using System.Drawing;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLGameDemo.Windows;

internal class GraphicsBatcher2DTestWindow(IWindow w) : GLGameWindow(w)
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
    static readonly Vertex2D.InstanceTransform2D[] _instanceTransforms =
    [
        new() { Position = new(10, 10), Size = new(50, 50), ZIndex = 0, Color = Color.Red.ToVector4() },
        new() { Position = new(40, 40), Size = new(50, 50), ZIndex = 1, Color = Color.Green.ToVector4() },
        new() { Position = new(10, 20), Size = new(75, 75), ZIndex = 2, Color = Color.Blue.ToVector4() },
        new() { Position = new(30, 5), Size = new(50, 50), ZIndex = 0, Color = Color.Yellow.ToVector4() },
        new() { Position = new(120, 120), Size = new(50, 50), ZIndex = 4, Color = Color.Cyan.ToVector4() },
        new() { Position = new(240, 240), Size = new(50, 50), ZIndex = 5, Color = Color.Magenta.ToVector4() },
        new() { Position = new(180, 180), Size = new(50, 50), ZIndex = 6, Color = Color.Orange.ToVector4() },
        new() { Position = new(160, 160), Size = new(100, 100), ZIndex = 7, Color = Color.Purple.ToVector4() },
        new() { Position = new(120, 240), Size = new(50, 50), ZIndex = 8, Color = Color.Lime.ToVector4() },
    ];

    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Graphics Batcher 2D Test Window";
        _gL.Enable(EnableCap.DepthTest);

        IVertexArray vertexArray = Graphics.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

        ITexture2D texture2D = Graphics.CreateTexture2D();
        texture2D.Initialize(Vector2D<uint>.One, TextureFormat.Rgba8);
        texture2D.SetData(Vector2D<uint>.One, Vector2D<int>.Zero, stackalloc byte[] { 255, 255, 255, 255 });
        texture2D.BindTexture(0);

        vertexBuffer.Initialize(_vertices.Length, (uint)BufferStorageMask.None, _vertices);
        indexBuffer.Initialize(_indices.Length, (uint)BufferStorageMask.None, _indices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

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

        for (int i = 0; i < _instanceTransforms.Length; i++)
            graphicsBatcher?.DrawInstance(_instanceTransforms[i]);

        graphicsBatcher?.Flush();
    }

    protected override void OnUpdate(double deltaTime) { }
}