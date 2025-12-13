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

    public Matrix4X4<float>[] LightSpaceMatrices = new Matrix4X4<float>[5];
    public float[] CascadePlaneDistances = [10, 20, 50, 250];
    public int ShadowCascadeLevels = 4;

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

        CascadePlaneDistances[0] = PerspectiveCamera.FarPlane / 50;
        CascadePlaneDistances[1] = PerspectiveCamera.FarPlane / 20;
        CascadePlaneDistances[2] = PerspectiveCamera.FarPlane / 10;
        CascadePlaneDistances[3] = PerspectiveCamera.FarPlane / 4;
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
            PerspectiveCamera!.GetViewMatrix(out CameraViewMatrix);
            PerspectiveCamera.GetProjectionMatrix(out CameraProjectionMatrix);

            UpdateLightSpaceMatrices();
        }
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

    private Matrix4X4<float> GetLightSpaceMatrix(float nearPlane, float farPlane)
    {
        Matrix4X4<float> projection = Matrix4X4.CreatePerspectiveFieldOfView(
            PerspectiveCamera.FieldOfView, PerspectiveCamera.AspectRatio, nearPlane, farPlane);

        Matrix4X4.Invert(CameraViewMatrix * projection, out var inv);
        Vector3D<float> center = Vector3D<float>.Zero;

        Span<Vector4D<float>> vector4Ds = stackalloc Vector4D<float>[8];

        int index = 0;
        for (int x = 0; x < 2; ++x)
        {
            for (int y = 0; y < 2; ++y)
            {
                for (int z = 0; z < 2; ++z)
                {
                    Vector4D<float> pt = new Vector4D<float>
                    (
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        z, // 2.0f * z - 1.0f, // 1.0f * z,
                        1.0f
                    ) * inv;

                    pt /= pt.W;
                    center += new Vector3D<float>(pt.X, pt.Y, pt.Z);

                    vector4Ds[index++] = pt;
                }
            }
        }

        center /= 8;
        Matrix4X4<float> lightViewMatrix = Matrix4X4.CreateLookAt(
            center, center + DirectionalLight.LightDirection, Vector3D<float>.UnitY);

        float minX = float.MaxValue;
        float minY = float.MaxValue;
        float minZ = float.MaxValue;

        float maxX = float.MinValue;
        float maxY = float.MinValue;
        float maxZ = float.MinValue;

        for (int i = 0; i < 8; i++)
        {
            Vector4D<float> transform = vector4Ds[i] * lightViewMatrix;
            minX = MathF.Min(minX, transform.X);
            minY = MathF.Min(minY, transform.Y);
            minZ = MathF.Min(minZ, transform.Z);

            maxX = MathF.Max(maxX, transform.X);
            maxY = MathF.Max(maxY, transform.Y);
            maxZ = MathF.Max(maxZ, transform.Z);
        }

        // Tune this parameter according to the scene
        const float zMult = 2.0f;
        if (minZ < 0)
            minZ *= zMult;
        else
            minZ /= zMult;

        if (maxZ < 0)
            maxZ /= zMult;
        else
            maxZ *= zMult;

        return lightViewMatrix * Matrix4X4.CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
    }
}
