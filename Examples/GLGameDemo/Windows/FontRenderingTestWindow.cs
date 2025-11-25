using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using System.Drawing;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.Graphics.Text.Extensions;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLGameDemo.Windows;

internal class FontRenderingTestWindow(IWindow w) : GLGameWindow(w)
{
    static readonly (ShaderType, string)[] _gLSLSources =
    [
        (ShaderType.VertexShader, "Assets/Shaders/2D/sdf_font.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/2D/sdf_font.frag"),
    ];

    static readonly uint[] _indices = [0, 1, 3, 1, 2, 3];
    static readonly Vertex2D[] _vertices =
    [
        new() { Position = new(-1, -1) },
        new() { Position = new(-1, 1) },
        new() { Position = new(1, 1) },
        new() { Position = new(1, -1) },
    ];

    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Font Rendering Test";

        _gL.Enable(EnableCap.DepthTest);
        _gL.Enable(EnableCap.Blend);
        _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

        IVertexArray vertexArray = Graphics.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

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
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.Black);

        _gL.DepthMask(false);

        graphicsBatcher?.DrawText
        (
            """
            每一个字形都放在一个水平的基准线(Baseline)上（即上图中水平箭头指示的那条线）。
            一些字形恰好位于基准线上（如’X’），而另一些则会稍微越过基准线以下（如’g’或’p’）
            （译注：即这些带有下伸部的字母，可以见这里）。
            这些度量值精确定义了摆放字形所需的每个字形距离基准线的偏移量，
            每个字形的大小，以及需要预留多少空间来渲染下一个字形。下面这个表列出了我们需要的所有属性。
            """,
            new Vector2D<float>(32, 32),
            GameApplication.Unifont
        );

        graphicsBatcher?.DrawText
        (
            """
            微软雅黑（英语：Microsoft YaHei）
            """,
            new Vector2D<float>(32, 144),
            GameApplication.Msyh,
            64
        );

        graphicsBatcher?.DrawText
        (
            """
            _gL.Enable(EnableCap.DepthTest);
            _gL.Enable(EnableCap.Blend);
            _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

            IVertexArray vertexArray = Graphics.CreateVertexArray();
            IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
            IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

            vertexBuffer.Initialize(_vertices.Length, (uint)BufferStorageMask.None, _vertices);
            indexBuffer.Initialize(_indices.Length, (uint)BufferStorageMask.None, _indices);
            vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);
            _gL.Enable(EnableCap.DepthTest);
            _gL.Enable(EnableCap.Blend);
            _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

            IVertexArray vertexArray = Graphics.CreateVertexArray();
            IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
            IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

            vertexBuffer.Initialize(_vertices.Length, (uint)BufferStorageMask.None, _vertices);
            indexBuffer.Initialize(_indices.Length, (uint)BufferStorageMask.None, _indices);
            vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);
            """,
            new Vector2D<float>(32, 256),
            GameApplication.JetbrainsMono
        );

        graphicsBatcher?.Flush();
        _gL.DepthMask(true);
    }
}
