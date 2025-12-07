using Silk.NET.Maths;
using Silk.NET.Windowing;
using Vertix.Engine.Camera;
using Vertix.Engine.Scene;
using Vertix.Engine.Scene.Lighting;

//using Vertix.Extensions;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering;

internal class RenderContext
{
    private readonly IWindow CoreWindow;

    public Rectangle<int> WindowRectangle;
    //public Matrix4X4<float> WindowMatrix;
    //public Matrix4X4<float> WindowViewMatirx = Matrix4X4<float>.Identity;
    //public Matrix4X4<float> WindowProjectionMatrix;

    public Matrix4X4<float> CameraViewMatrix;
    public Matrix4X4<float> CameraProjectionMatrix;

    public PerspectiveCamera PerspectiveCamera { get; } = new();

    public SceneManager SceneManager { get; } = new();

    public DirectionalLight DirectionalLight { get; } = new() 
    {
        Orientation = Quaternion<float>.CreateFromYawPitchRoll(-Scalar<float>.PiOver2 / 2, -Scalar<float>.PiOver2 / 2, 0),
    };

    public IRenderTarget? GBufferTarget { get; set; }

    public IRenderTarget? DirectionalShadowTarget { get; set; }

    public IRenderTarget? DirectionalLightingTarget { get; set; }

    public RenderContext(IWindow window)
    {
        CoreWindow = window;
        CoreWindow.Resize += OnWindowResized;

        OnWindowResized(CoreWindow.Size);
    }

    private void OnWindowResized(Vector2D<int> obj)
    {
        WindowRectangle = new Rectangle<int>(0, 0, CoreWindow.Size.X, CoreWindow.Size.Y);
        //WindowMatrix = WindowRectangle.ToScreenMatrix();
        //WindowProjectionMatrix = Matrix4X4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);

        PerspectiveCamera.AspectRatio = CoreWindow.Size.X / (float)CoreWindow.Size.Y;
    }

    public void Update()
    {
        if (PerspectiveCamera != null)
        {
            PerspectiveCamera!.GetViewMatrix(out var view);
            PerspectiveCamera.GetProjectionMatrix(out var projection);
            CameraViewMatrix = view;
            CameraProjectionMatrix = projection;
        }
    }
}
