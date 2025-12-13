using System.Numerics;

namespace Vertix.Engine.Scene.Lighting;

public class DirectionalLight : GameObject3D
{
    public Matrix4x4 LightViewMatrix { get; private set; }

    public Matrix4x4 LightProjectionMatrix { get; private set; }

    public Matrix4x4 LightViewProjectionMatrix { get; private set; }

    public Vector3 LightDirection { get; private set; }

    public float NearPlane { get; set; } = 1.0f;

    public float FarPlane { get; set; } = 20f;

    public float OrthographicSize { get; set; } = 20f;

    protected override void OnWorldMatrixChanged()
    {
        Vector3 forward = Vector3.Transform(-Vector3.UnitZ, Orientation);
        Vector3 up = Vector3.Transform(Vector3.UnitY, Orientation);
        Vector3 target = Position + forward;

        LightDirection = forward;
        LightViewMatrix = Matrix4x4.CreateLookAt(Position, target, up);
        LightProjectionMatrix = Matrix4x4.CreateOrthographic(OrthographicSize, OrthographicSize, NearPlane, FarPlane);
        LightViewProjectionMatrix = LightViewMatrix * LightProjectionMatrix;
    }

    public void FitToScene(Vector3 sceneCenter, float sceneRadius)
    {
        OrthographicSize = sceneRadius * 2.0f;
        FarPlane = sceneRadius * 3.0f;
        Position = sceneCenter - LightDirection * sceneRadius * 1.5f;
    }
}