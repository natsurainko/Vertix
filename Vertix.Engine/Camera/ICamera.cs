using Silk.NET.Maths;

namespace Vertix.Engine.Camera;

public interface ICamera
{
    Vector3D<float> Position { get; set; }
    Quaternion<float> Orientation { get; set; }

    CameraMode CameraMode { get; set; }
    float TargetDistance { get; set; }

    float FieldOfView { get; }
    float AspectRatio { get; }
    float NearPlane { get; }
    float FarPlane { get; }

    //GameObject3D? Target { get; }

    void GetViewMatrix(out Matrix4X4<float> matrix);
    void GetProjectionMatrix(out Matrix4X4<float> matrix);

    void Move(Vector3D<float> offset);
    void Rotate(Vector3D<float> angles);

    //void SetTarget(GameObject3D? target);
}
