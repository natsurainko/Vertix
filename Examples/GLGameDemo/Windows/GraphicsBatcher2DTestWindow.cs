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
    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    Vertex2D.InstanceTransform2D[] instanceTransform = [];
    readonly (Rectangle<float>, float, Color)[] rectangles =
    [
        (new(10, 10, 50, 50), 0, Color.Red),
        (new(40, 40, 50, 50), 1, Color.Green),
        (new(10, 45, 50, 50), 2, Color.Blue),
        (new(30, 5, 50, 50), 0, Color.Yellow),
        (new(120, 120, 50, 50), 4, Color.Cyan),
        (new(240, 240, 50, 50), 5, Color.Magenta),
        (new(180, 180, 50, 50), 6, Color.Orange),
        (new(160, 160, 100, 100), 7, Color.Purple),
        (new(120, 240, 50, 50), 8, Color.Lime),
    ];

    Matrix4X4<float> view = Matrix4X4<float>.Identity;
    Matrix4X4<float> projection = Matrix4X4.CreateOrthographicOffCenter(0, 800, 600, 0, -100f, 100f);
    Matrix4X4<float> instanceMatrix;

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

        vertexBuffer.Initialize(GameApplication.RectangleVertices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleVertices);
        indexBuffer.Initialize(GameApplication.RectangleIndices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleIndices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

        graphicsBatcher = Graphics.CreateGraphicsBatcher<Vertex2D.InstanceTransform2D>(in vertexArray,
            Vertex2D.InstanceTransform2D.DefaultProperties, (uint)GameApplication.RectangleIndices.Length);

        instanceTransform = [.. rectangles.Select(r => new Vertex2D.InstanceTransform2D()
        {
            Color = r.Item3.ToVector4(),
            WorldMatirx = r.Item1.ToScreenMatrix(r.Item2)
        })];

        shader = Graphics.CreateShaderProgram();
        shader.LoadGLSLShadersFromFiles(GameApplication._2D_BASIC_SHADER);
        shader.Compile();

        Graphics.UseShaderProgram(shader);

        shader?.Parameters["view"].SetValue(view);
        shader?.Parameters["projection"].SetValue(projection);

        CoreWindow.Resize += CoreWindow_Resize;
    }

    private void CoreWindow_Resize(Vector2D<int> size)
    {
        Graphics.Viewport(size);

        projection = Matrix4X4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);
        shader?.Parameters["projection"].SetValue(projection);
    }

    protected unsafe override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.CornflowerBlue);

        for (int i = 0; i < instanceTransform.Length; i++)
            graphicsBatcher?.DrawInstance(instanceTransform[i]);

        graphicsBatcher?.Flush();
    }

    protected override void OnUpdate(double deltaTime) { }
}