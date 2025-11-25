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

namespace GLGameDemo.Windows;

internal class DrawInstanceTestWindow(IWindow w, IServiceProvider sp) : GLGameWindow(w)
{
    static readonly (ShaderType, string)[] _gLSLSources =
    [
        (ShaderType.VertexShader, "Assets/Shaders/3D/triangles.vert"),
        (ShaderType.FragmentShader, "Assets/Shaders/3D/triangles.frag"),
    ];

    const int blockCountAsix = 128;

    Model model;
    PerspectiveCamera Camera = new();
    IShaderProgram? shader;

    protected unsafe override void OnLoaded()
    {
        CoreWindow.Title = "Draw Instance Test Window";
        _gL.Enable(EnableCap.DepthTest);

        Camera.Position = new Vector3D<float>(-15f, 15f, 15f);
        Camera.Rotate(new Vector3D<float>(-Scalar<float>.Pi / 10, -Scalar<float>.PiOver2 / 2, .0f));

        var assetImporter = sp.GetRequiredService<AssetImporter>();
        model = assetImporter.LoadModel("Assets/Models/block.fbx");

        Graphics.InitializeModelMeshesVertexArray(model);

        Vertex.InstanceTransform3D[] instanceTransforms = new Vertex.InstanceTransform3D[blockCountAsix * blockCountAsix];
        float spacing = 1.5f;
        for (int i = 0; i < blockCountAsix; i++)
        {
            for (int j = 0; j < blockCountAsix; j++)
            {
                instanceTransforms[i * blockCountAsix + j].WorldMatirx = Matrix4X4.CreateWorld
                (
                    new Vector3D<float>(i * spacing, 0, -j * spacing),
                    -Vector3D<float>.UnitZ,
                    Vector3D<float>.UnitY
                );
            }
        }

        IGraphicsBuffer instanceBuffer = Graphics.CreateGraphicsBuffer();
        instanceBuffer.Initialize(blockCountAsix * blockCountAsix, (uint)BufferStorageMask.None, instanceTransforms);

        for (int i = 0; i < model.Meshes.Length; i++)
            model.Meshes[i].VertexArray?.AttachInstanceBuffer<Vertex.InstanceTransform3D>(instanceBuffer, Vertex.InstanceTransform3D.DefaultProperties);

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
                (uint)mesh.Indices.Length, blockCountAsix * blockCountAsix);
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
