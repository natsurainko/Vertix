using Silk.NET.Maths;
using Silk.NET.Windowing;
using System.Numerics;
using Vertix.Engine.Camera;
using Vertix.Engine.Extensions;
using Vertix.Engine.Scene;
using Vertix.Engine.Scene.Lighting;
using Vertix.Rendering;

namespace GLRenderPipelineDemo.Rendering;

internal class RenderContext
{
    private readonly IWindow CoreWindow;

    public Rectangle<int> WindowRectangle;
    public Matrix4x4 CameraViewMatrix;
    public Matrix4x4 CameraProjectionMatrix;

    public Matrix4x4[] LightSpaceMatrices = new Matrix4x4[5];
    public float[] CascadePlaneDistances = [10, 20, 50, 250];
    public int ShadowCascadeLevels = 4;

    public PerspectiveCamera PerspectiveCamera { get; } = new();

    public SceneManager SceneManager { get; } = new();

    public DirectionalLight DirectionalLight { get; } = new()
    {
        Orientation = Quaternion.CreateFromYawPitchRoll(-MathF.PI / 4, -MathF.PI / 4, 0),
    };

    public IRenderTarget? GBufferTarget { get; set; }

    public IRenderTarget? DirectionalShadowTarget { get; set; }

    public IRenderTarget? DirectionalLightingTarget { get; set; }

    public IRenderTarget? AmbientOcclusionTarget { get; set; }

    public bool EnableAmbientOcclusion { get; set; } = true;

    public Vector2D<int> ShadowMapSize { get; init; } = new(2048, 2048);

    public RenderContext(IWindow window)
    {
        CoreWindow = window;
        CoreWindow.Resize += OnWindowResized;

        OnWindowResized(CoreWindow.Size);

        CascadePlaneDistances[0] = PerspectiveCamera.FarPlane / 50;
        CascadePlaneDistances[1] = PerspectiveCamera.FarPlane / 20;
        CascadePlaneDistances[2] = PerspectiveCamera.FarPlane / 10;
        CascadePlaneDistances[3] = PerspectiveCamera.FarPlane / 4;
    }

    private void OnWindowResized(Vector2D<int> obj)
    {
        WindowRectangle = new Rectangle<int>(0, 0, CoreWindow.Size.X, CoreWindow.Size.Y);
        PerspectiveCamera.AspectRatio = CoreWindow.Size.X / (float)CoreWindow.Size.Y;
    }

    public void Update()
    {
        if (PerspectiveCamera != null)
        {
            PerspectiveCamera!.GetViewMatrix(out CameraViewMatrix);
            PerspectiveCamera.GetProjectionMatrix(out CameraProjectionMatrix);

            UpdateLightSpaceMatrices();
        }

        //DirectionalLight.Orientation = Quaternion.CreateFromYawPitchRoll(-MathF.PI / 4, MathF.PI * (float)Math.Sin(CoreWindow.Time * 0.1), 0);
    }

    private void UpdateLightSpaceMatrices()
    {
        for (int i = 0; i < CascadePlaneDistances.Length + 1; i++)
        {
            float nearPlane = PerspectiveCamera.NearPlane;
            float farPlane = PerspectiveCamera.FarPlane;

            if (i == 0)
                farPlane = CascadePlaneDistances[i];
            else if (i < CascadePlaneDistances.Length)
            {
                nearPlane = CascadePlaneDistances[i - 1];
                farPlane = CascadePlaneDistances[i];
            }
            else
                nearPlane = CascadePlaneDistances[i - 1];

            LightSpaceMatrices[i] = Matrix4x4.CreateLightViewProjectionForFrustum(DirectionalLight.LightDirection, 
                CameraViewMatrix * Matrix4x4.CreatePerspectiveFieldOfView(PerspectiveCamera.FieldOfView, PerspectiveCamera.AspectRatio, nearPlane, farPlane));
        }
    }
}
