using System.Numerics;

namespace Vertix.Engine.Camera;

public interface ICamera
{
    Vector3 Position { get; set; }
    Quaternion Orientation { get; set; }

    CameraMode CameraMode { get; set; }
    float TargetDistance { get; set; }

    float FieldOfView { get; }
    float AspectRatio { get; }
    float NearPlane { get; }
    float FarPlane { get; }

    GameObject3D? Target { get; }

    void GetViewMatrix(out Matrix4x4 matrix);
    void GetProjectionMatrix(out Matrix4x4 matrix);

    void Move(Vector3 offset);
    void Rotate(Vector3 angles);

    void SetTarget(GameObject3D? target);
}
