using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using SkiaSharp;
using System.Drawing;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;

namespace GLGameDemo.Windows;

internal class TextureAtlasTestWindow(IWindow w) : GLGameWindow(w)
{
    static readonly (ShaderType, string)[] _gLSLSources =
    [
        (ShaderType.VertexShader, "Assets/Shaders/2D/rectangle.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/2D/rectangle.frag"),
    ];

    static readonly uint[] _indices = [0, 1, 3, 1, 2, 3];
    static readonly Vertex2D.InstanceTransform2D[] _instances = new Vertex2D.InstanceTransform2D[4];
    static readonly Vertex2D[] _vertices =
    [
        new() { Position = new(-1, -1) },
        new() { Position = new(-1, 1) },
        new() { Position = new(1, 1) },
        new() { Position = new(1, -1) },
    ];

    Vector2D<float> _instancePosition = new(32, 32);
    Vector2D<float> _instanceSize = new(256, 256);

    int _currentInstanceIndex;

    IShaderProgram? shader;
    IGraphicsBatcher<Vertex2D.InstanceTransform2D>? graphicsBatcher;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Texture Atlas Test";

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

        using (var bitmap = SKBitmap.Decode("Assets/chicken.png"))
        {
            Vector2D<uint> size = new((uint)bitmap.Width, (uint)bitmap.Height);

            texture.Initialize(size, TextureFormat.Bgra8);
            texture.SetData(size, Vector2D<int>.Zero, bitmap.GetPixelSpan());
            texture.BindTexture(0);

            Vector2D<float> tiledSize = new(16, 16);

            for (int i = 0; i < _instances.Length; i++)
            {
                _instances[i] = new Vertex2D.InstanceTransform2D
                {
                    Position = _instancePosition,
                    Size = _instanceSize,
                    TextureOffset = new(i * tiledSize.X / bitmap.Width, 0),
                    TextureScale = new(tiledSize.X / bitmap.Width, tiledSize.Y / bitmap.Height),
                };
            }

            _currentInstanceIndex = 0;
            _instanceSize = Vector2D<float>.One * (float)(Math.Min(CoreWindow.Size.X, CoreWindow.Size.Y) / 2.0);
            _instancePosition.Y = (CoreWindow.Size.Y - _instanceSize.Y) / 2f;
        }

        ITextureSampler textureSampler = Graphics.CreateTextureSampler();
        textureSampler.MinFilter = TextureFilter.Nearest;
        textureSampler.MagFilter = TextureFilter.Nearest;
        textureSampler.AddressU = TextureAddressMode.ClampToEdge;
        textureSampler.AddressV = TextureAddressMode.ClampToEdge;
        textureSampler.AddressW = TextureAddressMode.ClampToEdge;
        textureSampler.BindSampler(0);

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

        _instanceSize = Vector2D<float>.One * MathF.Min(CoreWindow.Size.X, CoreWindow.Size.Y) / 2f;
        _instancePosition.Y = (CoreWindow.Size.Y - _instanceSize.Y) / 2f;
    }

    protected unsafe override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.CornflowerBlue);

        Vertex2D.InstanceTransform2D frameInstance = _instances[_currentInstanceIndex];
        frameInstance.Size = _instanceSize;
        frameInstance.Position = _instancePosition;

        graphicsBatcher?.DrawInstance(frameInstance);
        graphicsBatcher?.Flush();
    }

    double _deltaTime = 0;

    protected override void OnUpdate(double deltaTime) 
    {
        _deltaTime += deltaTime;

        if (_deltaTime >= 0.25)
        {
            _currentInstanceIndex = (_currentInstanceIndex + 1) % 4;
            _instancePosition.X = (_instancePosition.X +( _instanceSize.X / 8) + _instanceSize.X) % (CoreWindow.Size.X + _instanceSize.X) - _instanceSize.X;
            _deltaTime = 0;
        }
    }
}
