using Microsoft.Extensions.DependencyInjection;
using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using System.Drawing;
using Vertix.Content;
using Vertix.Engine.Camera;
using Vertix.Graphics;
using Vertix.Graphics.Resources;
using Vertix.OpenGL.Extensions;
using Vertix.OpenGL.Windowing;
using Vertix.Rendering;
using ClearBufferMask = Vertix.Graphics.ClearBufferMask;
using PrimitiveType = Vertix.Graphics.PrimitiveType;

namespace GLGameDemo;

internal class MainGameWindow(IWindow w, IServiceProvider sp) : GLGameWindow(w)
{
    static readonly (ShaderType, string)[] _gLSLSources =
    [
        (ShaderType.VertexShader, "Assets/Shaders/triangles.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/triangles.frag"),
    ];

    int blockCount = 10000;

    Model model;
    PerspectiveCamera Camera = new();
    IShaderProgram? shader;

    protected unsafe override void OnLoaded()
    {
        _gL.Enable(EnableCap.DepthTest);

        Camera.Position = new Vector3D<float>(-15f, 15f, 15f);
        Camera.Rotate(new Vector3D<float>(-Scalar<float>.Pi / 10, -Scalar<float>.PiOver2 / 2, .0f));

        var assetImporter = sp.GetRequiredService<AssetImporter>();
        model = assetImporter.LoadModel("Assets/block.fbx");

        Graphics.InitializeModelMeshesVertexArray(model);

        Matrix4X4<float>[] instanceTransforms = new Matrix4X4<float>[blockCount];
        float spacing = 1.5f;
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 100; j++)
            {
                Matrix4X4<float> transform = Matrix4X4.CreateWorld
                (
                    new Vector3D<float>(i * spacing, 0, -j * spacing),
                    -Vector3D<float>.UnitZ,
                    Vector3D<float>.UnitY
                );
                instanceTransforms[i * 100 + j] = transform;
            }
        }

        IGraphicsBuffer instanceBuffer = Graphics.CreateGraphicsBuffer();
        instanceBuffer.Initialize(blockCount, (uint)BufferStorageMask.None, instanceTransforms);

        ReadOnlySpan<VertexArrayProperty> properties =
        [
            new VertexArrayProperty<float>(4, 2),
            new VertexArrayProperty<float>(4, 3),
            new VertexArrayProperty<float>(4, 4),
            new VertexArrayProperty<float>(4, 5)
        ];

        for (int i = 0; i < model.Meshes.Length; i++)
            model.Meshes[i].VertexArray?.AttachInstanceBuffer<Matrix4X4<float>>(instanceBuffer, properties);

        shader = Graphics.CreateShaderProgram();
        shader.LoadGLSLShadersFromFiles(_gLSLSources);
        shader.Compile();

        Graphics.UseShaderProgram(shader);

        CoreWindow.Resize += CoreWindow_Resize;
    }

    private void CoreWindow_Resize(Vector2D<int> size)
    {
        Graphics.Viewport(size);
        Camera.AspectRatio = (float)size.X / size.Y;
    }

    protected unsafe override void OnRender(double delateTime)
    {
        Graphics.Clear(ClearBufferMask.Color | ClearBufferMask.Depth, Color.CornflowerBlue);

        for (int i = 0; i < model.Meshes.Length; i++)
        {
            Mesh mesh = model.Meshes[i];
            Graphics.DrawVertexElementsArrayInstanced(in mesh.VertexArray!, PrimitiveType.Triangles,
                (uint)mesh.Indices.Length, (uint)blockCount);
        }
    }

    protected override void OnUpdate(double deltaTime) 
    {
        Camera.GetViewMatrix(out var view);
        Camera.GetProjectionMatrix(out var projection);

        shader?.Parameters["view"].SetValue(view);
        shader?.Parameters["projection"].SetValue(projection);
    }
}
