using GLGameDemo.Extensions;
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

internal class RenderTargetTestWindow(IWindow w) : GLGameWindow(w)
{
    IShaderProgram? fontShader;
    IShaderProgram? rectShader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    ITexture2D? texture2D;
    ITextureSampler? textureSampler;
    IRenderTarget? renderTarget;

    Matrix4X4<float> view = Matrix4X4<float>.Identity;
    Matrix4X4<float> projection = Matrix4X4.CreateOrthographicOffCenter(0, 800, 600, 0, -100f, 100f);
    Matrix4X4<float> instanceMatrix;

    bool _needRecreateRenderTarget = true;
    bool _needRedrawRenderTarget;
    bool _resizing = false;
    Action? _resizedAction;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Render Target Test";
        CoreWindow.ShouldSwapAutomatically = false;

        _gL.Enable(EnableCap.DepthTest);
        _gL.Enable(EnableCap.Blend);
        _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

        IVertexArray vertexArray = Graphics.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

        vertexBuffer.Initialize(GameApplication.RectangleVertices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleVertices);
        indexBuffer.Initialize(GameApplication.RectangleIndices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleIndices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

        graphicsBatcher = Graphics.CreateGraphicsBatcher<Vertex2D.InstanceTransform2D>(in vertexArray,
            Vertex2D.InstanceTransform2D.DefaultProperties, (uint)GameApplication.RectangleIndices.Length);

        textureSampler = Graphics.CreateTextureSampler();
        textureSampler.MinFilter = TextureFilter.Nearest;
        textureSampler.MagFilter = TextureFilter.Nearest;
        textureSampler?.BindSampler(0);

        instanceMatrix = new Rectangle<float>(0, 0, CoreWindow.Size.X, CoreWindow.Size.Y).ToScreenMatrix();

        PrepareShaders();
        Graphics.UseShaderProgram(rectShader);

        _resizedAction = () => _resizing = false;
        _resizedAction = _resizedAction.Debounce(125);
        CoreWindow.Resize += CoreWindow_Resize;
    }

    private void CoreWindow_Resize(Vector2D<int> size)
    {
        if (CoreWindow.WindowState == WindowState.Minimized)
            return;

        Graphics.Viewport(size);

        projection = Matrix4X4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);
        instanceMatrix = new Rectangle<float>(0, 0, CoreWindow.Size.X, CoreWindow.Size.Y).ToScreenMatrix();

        rectShader?.Parameters["projection"].SetValue(projection);
        fontShader?.Parameters["projection"].SetValue(projection);

        if (new Vector2D<uint>((uint)CoreWindow.Size.X, (uint)CoreWindow.Size.Y) == renderTarget?.Size)
            return;

        _needRecreateRenderTarget = true;
        _resizing = true;

        _resizedAction?.Invoke();
    }

    protected unsafe override void OnRender(double delateTime)
    {
        if (_resizing)
            return;

        if (_needRecreateRenderTarget)
        {
            PrepareRenderTarget();
            _needRecreateRenderTarget = false;
        }

        if (_needRedrawRenderTarget)
        {
            Graphics.BindRenderTarget(renderTarget);
            Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.Black);
            _gL.DepthMask(false);

            Graphics.UseShaderProgram(fontShader);

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
                GameApplication.Unifont!
            );

            graphicsBatcher?.DrawText
            (
                """
                微软雅黑（英语：Microsoft YaHei）
                """,
                new Vector2D<float>(32, 144),
                GameApplication.Msyh!,
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
                GameApplication.JetbrainsMono!
            );

            _gL.DepthMask(true);
            _needRedrawRenderTarget = false;

            Graphics.BindRenderTarget(null);
            Graphics.UseShaderProgram(rectShader);
            texture2D?.BindTexture(0);
        }

        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.Black);

        graphicsBatcher?.DrawInstance(new() { WorldMatirx = instanceMatrix, TextureRegion = new Vector4D<float>(0, 0, 1, -1) });
        graphicsBatcher?.Flush();

        CoreWindow.SwapBuffers();
    }

    private void PrepareRenderTarget()
    {
        renderTarget?.Dispose();
        texture2D?.Dispose();

        Vector2D<uint> size = new((uint)CoreWindow.Size.X, (uint)CoreWindow.Size.Y);
        texture2D = Graphics.CreateTexture2D();
        texture2D.Initialize(size, TextureFormat.Rgba32f);
        renderTarget = Graphics.CreateRenderTarget(size);
        renderTarget.AttachTargetTexture(texture2D);
        renderTarget.Initialize();

        _needRedrawRenderTarget = true;
    }

    private void PrepareShaders()
    {
        fontShader = Graphics.CreateShaderProgram();
        fontShader.LoadGLSLShadersFromFiles(GameApplication._2D_FONT_SHADER);
        fontShader.Compile();
        fontShader?.Parameters["view"].SetValue(view);
        fontShader?.Parameters["projection"].SetValue(projection);

        rectShader = Graphics.CreateShaderProgram();
        rectShader.LoadGLSLShadersFromFiles(GameApplication._2D_BASIC_SHADER);
        rectShader.Compile();
        rectShader?.Parameters["view"].SetValue(view);
        rectShader?.Parameters["projection"].SetValue(projection);
    }
}
