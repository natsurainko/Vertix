using Silk.NET.Maths;

namespace Vertix.Engine.Scene.Lighting;

public class DirectionalLight : GameObject3D
{
    public Matrix4X4<float> LightViewMatrix { get; private set; }

    public Matrix4X4<float> LightProjectionMatrix { get; private set; }

    public Matrix4X4<float> LightViewProjectionMatrix { get; private set; }

    public Vector3D<float> LightDirection { get; private set; }

    public float NearPlane { get; set; } = 1.0f;

    public float FarPlane { get; set; } = 20f;

    public float OrthographicSize { get; set; } = 20f;

    protected override void OnWorldMatrixChanged()
    {
        Vector3D<float> forward = Vector3D.Transform(-Vector3D<float>.UnitZ, Orientation);
        Vector3D<float> up = Vector3D.Transform(Vector3D<float>.UnitY, Orientation);
        Vector3D<float> target = Position + forward;

        LightDirection = forward;
        LightViewMatrix = Matrix4X4.CreateLookAt(Position, target, up);
        LightProjectionMatrix = Matrix4X4.CreateOrthographic(OrthographicSize, OrthographicSize, NearPlane, FarPlane);
        LightViewProjectionMatrix = LightViewMatrix * LightProjectionMatrix;
    }

    public void FitToScene(Vector3D<float> sceneCenter, float sceneRadius)
    {
        OrthographicSize = sceneRadius * 2.0f;
        FarPlane = sceneRadius * 3.0f;
        Position = sceneCenter - LightDirection * sceneRadius * 1.5f;
    }
}