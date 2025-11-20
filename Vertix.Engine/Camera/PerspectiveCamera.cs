using Silk.NET.Maths;

namespace Vertix.Engine.Camera;

public class PerspectiveCamera : ICamera
{
    //private readonly ViewportAdapter _viewportAdapter = viewportAdapter;
    //private GameObject3D? _targetObject;

    private Vector3D<float> _movementAccumulator = Vector3D<float>.Zero;
    private Vector3D<float> _rotationAccumulator = Vector3D<float>.Zero;

    public Vector3D<float> Position { get; set; }

    public Quaternion<float> Orientation { get; set; } = Quaternion<float>.Identity;

    public float FieldOfView { get; set; } = Scalar.DegreesToRadians(45f);
    public float AspectRatio { get; set; } = 800 / 600f;
    public float NearPlane { get; set; } = 0.1f;
    public float FarPlane { get; set; } = 1000f;

    public CameraMode CameraMode { get; set; } = CameraMode.Free;
    public float TargetDistance { get; set; } = 5f;

    public Vector3D<float> TargetPosition { get; set; } = Vector3D<float>.Zero;

    public void GetViewMatrix(out Matrix4X4<float> matrix)
    {
        if (_rotationAccumulator != Vector3D<float>.Zero)
        {
            Orientation *= Quaternion<float>.CreateFromYawPitchRoll(_rotationAccumulator.Y, _rotationAccumulator.X, _rotationAccumulator.Z);
            _rotationAccumulator = Vector3D<float>.Zero;
        }

        Quaternion<float> orientation = Orientation;// (_targetObject != null && CameraMode == CameraMode.FirstPerson ? _targetObject : this).GetOrientationForCamera();
        Vector3D<float> forward = Vector3D.Transform(-Vector3D<float>.UnitZ, orientation);
        Vector3D<float> up = Vector3D.Transform(Vector3D<float>.UnitY, orientation);

        Vector3D<float> cameraPosition;
        Vector3D<float> cameraTarget;

        if (CameraMode == CameraMode.FirstPerson)
        {
            cameraPosition = TargetPosition;
            cameraTarget = TargetPosition + forward;
        }
        else if (CameraMode == CameraMode.ThirdPerson)
        {
            cameraPosition = TargetPosition - forward * TargetDistance;
            cameraTarget = TargetPosition;
        }
        else
        {
            Vector3D<float> right = Vector3D.Transform(Vector3D<float>.UnitX, orientation);

            if (_movementAccumulator != Vector3D<float>.Zero)
            {
                // Apply changes to Position
                Position += forward * _movementAccumulator.X;
                Position += Vector3D<float>.UnitY * _movementAccumulator.Y;
                Position += right * _movementAccumulator.Z;

                // Reset accumulator
                _movementAccumulator = Vector3D<float>.Zero;
            }

            cameraPosition = Position;
            cameraTarget = Position + forward;
        }

        matrix = Matrix4X4.CreateLookAt(cameraPosition, cameraTarget, up);
    }

    public void GetProjectionMatrix(out Matrix4X4<float> matrix) => matrix = Matrix4X4.CreatePerspectiveFieldOfView(FieldOfView, AspectRatio, NearPlane, FarPlane);

    /// <summary>
    /// Move the camera in its relative orientation
    /// </summary>
    /// <param name="offset">offset == (forward, up, right)</param>
    public void Move(Vector3D<float> offset)
    {
        if (CameraMode != CameraMode.Free)
        {
            //_targetObject?.Move(offset);
            return;
        }

        _movementAccumulator += offset;
    }

    /// <summary>
    /// angles = (pitch, yaw, roll)
    /// </summary>
    /// <param name="angles"></param>
    public void Rotate(Vector3D<float> angles) => _rotationAccumulator += angles;

    //public void SetTarget(GameObject3D? target) => _targetObject = target;
}
