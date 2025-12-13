using System;
using System.Numerics;
using Vertix.Engine.Extensions;
using Vertix.Engine.Maths;

namespace Vertix.Engine.Camera;

public partial class PerspectiveCamera : GameObject3D
{
    /// <summary>
    /// Move the camera in its relative orientation
    /// </summary>
    /// <param name="offset">offset == (forward, up, right)</param>
    public override void Move(Vector3 offset)
    {
        if (CameraMode != CameraMode.Free)
        {
            _targetObject?.Move(offset);
            return;
        }

        _movementAccumulator += offset;
    }

    /// <summary>
    /// angles = (pitch, yaw, roll)
    /// </summary>
    /// <param name="angles"></param>
    public void Rotate(Vector3 angles) => _rotationAccumulator += angles;

    public override void Rotate(Vector3 angles, bool _ = false) => Rotate(angles);
}

public partial class PerspectiveCamera : ICamera
{
    private GameObject3D? _targetObject;

    private Vector3 _movementAccumulator = Vector3.Zero;
    private Vector3 _rotationAccumulator = Vector3.Zero;

    public float FieldOfView { get; set; } = float.DegreesToRadians(45f);
    public float AspectRatio { get; set; } = 800 / 600f;
    public float NearPlane { get; set; } = 0.1f;
    public float FarPlane { get; set; } = 500f;

    public CameraMode CameraMode { get; set; } = CameraMode.Free;
    public float TargetDistance { get; set; } = 5f;
    public bool AllowRoll { get; set; } = false;

    public Frustum Frustum { get; private set; }

    public GameObject3D? Target => _targetObject;

    public void GetViewMatrix(out Matrix4x4 matrix)
    {
        if (_rotationAccumulator != Vector3.Zero)
        {
            base.Rotate(_rotationAccumulator, AllowRoll);
            _rotationAccumulator = Vector3.Zero;
        }

        Quaternion orientation = _targetObject != null && CameraMode == CameraMode.FirstPerson ? _targetObject.Orientation : this.Orientation;
        Vector3 forward = Vector3.Transform(-Vector3.UnitZ, orientation);
        Vector3 up = Vector3.Transform(Vector3.UnitY, orientation);
        Vector3 right = Vector3.Transform(Vector3.UnitX, orientation);
        UpdateCameraFrustum(forward, up, right);

        Vector3 cameraPosition;
        Vector3 cameraTarget;

        if (CameraMode == CameraMode.FirstPerson && _targetObject != null)
        {
            cameraPosition = _targetObject.Position;
            cameraTarget = _targetObject.Position + forward;
        }
        else if (CameraMode == CameraMode.ThirdPerson && _targetObject != null)
        {
            cameraPosition = _targetObject.Position - forward * TargetDistance;
            cameraTarget = _targetObject.Position;
        }
        else
        {
            if (_movementAccumulator != Vector3.Zero)
            {
                // Apply changes to Position
                Position += forward * _movementAccumulator.X;
                Position += Vector3.UnitY * _movementAccumulator.Y;
                Position += right * _movementAccumulator.Z;

                // Reset accumulator
                _movementAccumulator = Vector3.Zero;
            }

            cameraPosition = Position;
            cameraTarget = Position + forward;
        }

        matrix = Matrix4x4.CreateLookAt(cameraPosition, cameraTarget, up);
    }

    public void GetProjectionMatrix(out Matrix4x4 matrix) => matrix = Matrix4x4.CreatePerspectiveFieldOfView(FieldOfView, AspectRatio, NearPlane, FarPlane);

    public void SetTarget(GameObject3D? target) => _targetObject = target;

    private void UpdateCameraFrustum(Vector3 forward, Vector3 up, Vector3 right)
    {
        float halfVSide = FarPlane * MathF.Tan(FieldOfView * .5f);
        float halfHSide = halfVSide * AspectRatio;
        Vector3 frontMultFar = FarPlane * forward;

        Frustum = new()
        {
            LeftPlane = Plane.Create(Position, Vector3.Cross(up, frontMultFar + right * halfHSide)),
            RightPlane = Plane.Create(Position, Vector3.Cross(frontMultFar - right * halfHSide, up)),

            TopPlane = Plane.Create(Position, Vector3.Cross(right, frontMultFar - up * halfVSide)),
            BottomPlane = Plane.Create(Position, Vector3.Cross(frontMultFar + up * halfVSide, right)),

            NearPlane = Plane.Create(Position + NearPlane * forward, forward),
            FarPlane = Plane.Create(Position + frontMultFar, forward),
        };
    }
}
