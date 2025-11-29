using Microsoft.Extensions.DependencyInjection;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using System.Drawing;
using Vertix.Content;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLGameDemo.Windows;

internal class TextureAtlasTestWindow(IWindow w, IServiceProvider sp) : GLGameWindow(w)
{
    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    int _texureAtlasIndex = 0;
    Rectangle<float> rect;

    Matrix4X4<float> view = Matrix4X4<float>.Identity;
    Matrix4X4<float> projection = Matrix4X4.CreateOrthographicOffCenter(0, 800, 600, 0, -100f, 100f);
    Matrix4X4<float> instanceMatrix;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Texture Atlas Test";

        _gL.Enable(EnableCap.DepthTest);
        _gL.Enable(EnableCap.Blend);
        _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

        IVertexArray vertexArray = Graphics.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

        vertexBuffer.Initialize(GameApplication.RectangleVertices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleVertices);
        indexBuffer.Initialize(GameApplication.RectangleIndices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleIndices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

        ITexture2D texture = sp.GetRequiredService<AssetImporter>().LoadImageTexture(Graphics, "Assets/Images/chicken.png");
        texture.BindTexture(0);

        ITextureSampler textureSampler = Graphics.CreateTextureSampler();
        textureSampler.MinFilter = TextureFilter.Nearest;
        textureSampler.MagFilter = TextureFilter.Nearest;
        textureSampler.AddressU = TextureAddressMode.ClampToEdge;
        textureSampler.AddressV = TextureAddressMode.ClampToEdge;
        textureSampler.AddressW = TextureAddressMode.ClampToEdge;
        textureSampler.BindSampler(0);

        graphicsBatcher = Graphics.CreateGraphicsBatcher<Vertex2D.InstanceTransform2D>(in vertexArray,
            Vertex2D.InstanceTransform2D.DefaultProperties, (uint)GameApplication.RectangleIndices.Length);

        shader = Graphics.CreateShaderProgram();
        shader.LoadGLSLShadersFromFiles(GameApplication._2D_BASIC_SHADER);
        shader.Compile();

        Graphics.UseShaderProgram(shader);

        rect.Size = Vector2D<float>.One * MathF.Min(CoreWindow.Size.X, CoreWindow.Size.Y) / 2f;
        rect.Origin.Y = (CoreWindow.Size.Y - rect.Size.Y) / 2f;
        instanceMatrix = rect.ToScreenMatrix();

        shader?.Parameters["view"].SetValue(view);
        shader?.Parameters["projection"].SetValue(projection);

        CoreWindow.Resize += CoreWindow_Resize;
    }

    private void CoreWindow_Resize(Vector2D<int> size)
    {
        Graphics.Viewport(size);

        projection = Matrix4X4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);
        shader?.Parameters["projection"].SetValue(projection);

        rect.Size = Vector2D<float>.One * MathF.Min(CoreWindow.Size.X, CoreWindow.Size.Y) / 2f;
        rect.Origin.Y = (CoreWindow.Size.Y - rect.Size.Y) / 2f;
    }

    protected unsafe override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.CornflowerBlue);

        graphicsBatcher?.DrawInstance(new() 
        {
            WorldMatirx = instanceMatrix,
            TextureRegion = new Vector4D<float>(0.25f * _texureAtlasIndex, 0, 0.25f, 1)
        });
        graphicsBatcher?.Flush();
    }

    double _deltaTime = 0;

    protected override void OnUpdate(double deltaTime) 
    {
        _deltaTime += deltaTime;

        if (_deltaTime >= 0.25)
        {
            _texureAtlasIndex = (_texureAtlasIndex + 1) % 4;

            rect.Origin.X = (rect.Origin.X + (rect.Size.X / 8) + rect.Size.X) % (CoreWindow.Size.X + rect.Size.X) - rect.Size.X;
            instanceMatrix = rect.ToScreenMatrix();
            _deltaTime = 0;
        }
    }
}
