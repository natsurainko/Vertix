using Silk.NET.Maths;
using Silk.NET.Windowing;
using System.Numerics;
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
    //public Matrix4x4 WindowMatrix;
    //public Matrix4x4 WindowViewMatirx = Matrix4x4.Identity;
    //public Matrix4x4 WindowProjectionMatrix;

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
        //WindowMatrix = WindowRectangle.ToScreenMatrix();
        //WindowProjectionMatrix = Matrix4x4.CreateOrthographicOffCenter(0, CoreWindow.Size.X, CoreWindow.Size.Y, 0, -100f, 100f);

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

            LightSpaceMatrices[i] = GetLightSpaceMatrix(nearPlane, farPlane);
        }
    }

    private Matrix4x4 GetLightSpaceMatrix(float nearPlane, float farPlane)
    {
        Matrix4x4 projection = Matrix4x4.CreatePerspectiveFieldOfView(
            PerspectiveCamera.FieldOfView, PerspectiveCamera.AspectRatio, nearPlane, farPlane);

        Matrix4x4.Invert(CameraViewMatrix * projection, out var inv);
        Vector3 center = Vector3.Zero;

        Span<Vector4> vector4Ds = stackalloc Vector4[8];

        int index = 0;
        for (int x = 0; x < 2; ++x)
        {
            for (int y = 0; y < 2; ++y)
            {
                for (int z = 0; z < 2; ++z)
                {
                    Vector4 pt = Vector4.Transform(new Vector4
                    (
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        z, // 2.0f * z - 1.0f, // 1.0f * z,
                        1.0f
                    ), inv);

                    pt /= pt.W;
                    center += new Vector3(pt.X, pt.Y, pt.Z);

                    vector4Ds[index++] = pt;
                }
            }
        }

        center /= 8;
        Matrix4x4 lightViewMatrix = Matrix4x4.CreateLookAt(
            center, center + DirectionalLight.LightDirection, Vector3.UnitY);

        float minX = float.MaxValue;
        float minY = float.MaxValue;
        float minZ = float.MaxValue;

        float maxX = float.MinValue;
        float maxY = float.MinValue;
        float maxZ = float.MinValue;

        for (int i = 0; i < 8; i++)
        {
            Vector4 transform = Vector4.Transform(vector4Ds[i], lightViewMatrix);
            minX = MathF.Min(minX, transform.X);
            minY = MathF.Min(minY, transform.Y);
            minZ = MathF.Min(minZ, transform.Z);

            maxX = MathF.Max(maxX, transform.X);
            maxY = MathF.Max(maxY, transform.Y);
            maxZ = MathF.Max(maxZ, transform.Z);
        }

        // Tune this parameter according to the scene
        const float zMult = 5.0f;
        if (minZ < 0)
            minZ *= zMult;
        else
            minZ /= zMult;

        if (maxZ < 0)
            maxZ /= zMult;
        else
            maxZ *= zMult;

        return lightViewMatrix * Matrix4x4.CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
    }
}
