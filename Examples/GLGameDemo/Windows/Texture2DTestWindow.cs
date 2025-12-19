using Microsoft.Extensions.DependencyInjection;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using System.Drawing;
using System.Numerics;
using Vertix.Engine.Content;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Primitives;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLGameDemo.Windows;

internal class Texture2DTestWindow(IWindow w, IGraphicsDevice d, IServiceProvider sp) : GLGameWindow(w, d)
{
    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    Rectangle<float> rect = new(100, 100, 224, 256);

    Matrix4x4 view = Matrix4x4.Identity;
    Matrix4x4 projection = Matrix4x4.CreateOrthographicOffCenter(0, 800, 600, 0, -100f, 100f);
    Matrix4x4 instanceMatrix;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Texture2D Test Window";

        _gL.Enable(EnableCap.Blend);
        _gL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

        IVertexArray vertexArray = Graphics.CreateVertexArray();
        IGraphicsBuffer vertexBuffer = Graphics.CreateGraphicsBuffer();
        IGraphicsBuffer indexBuffer = Graphics.CreateGraphicsBuffer();

        vertexBuffer.Initialize(GameApplication.RectangleVertices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleVertices);
        indexBuffer.Initialize(GameApplication.RectangleIndices.Length, (uint)BufferStorageMask.None, GameApplication.RectangleIndices);
        vertexArray.Initialize<Vertex2D>(vertexBuffer, Vertex2D.DefaultProperties, indexBuffer);

        ITexture2D texture = sp.GetRequiredService<AssetImporter>().LoadImageTexture(Graphics, "Assets/Images/Shed..png");
        Graphics.BindTexture(0, texture);

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

        instanceMatrix = rect.ToScreenMatrix();
        shader?.Parameters["view"].SetValue(view);
        shader?.Parameters["projection"].SetValue(projection);

        CoreWindow.Resize += CoreWindow_Resize;
    }

    private void CoreWindow_Resize(Vector2D<int> size)
    {
        Graphics.Viewport(new Rectangle<float>(Vector2D<float>.Zero, size.As<float>()));

        projection = Matrix4x4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);
        shader?.Parameters["projection"].SetValue(projection);
    }

    protected unsafe override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color, Color.CornflowerBlue);

        graphicsBatcher?.DrawInstance(new() { WorldMatirx = instanceMatrix });
        graphicsBatcher?.Flush();
    }

    protected override void OnUpdate(double deltaTime) { }
}
