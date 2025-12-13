using Silk.NET.Maths;
using Vertix.Graphics;

namespace Vertix.Engine;

public abstract partial class GameObject3D
{
    protected Matrix4X4<float> _worldMatrix;

    public virtual Matrix4X4<float> WorldMatrix => _worldMatrix;

    public Vector3D<float> Scale
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                UpdateWorldMatrix();
            }
        }
    } = Vector3D<float>.One;

    public Vector3D<float> Position
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                UpdateWorldMatrix();
            }
        }
    } = Vector3D<float>.Zero;

    public Quaternion<float> Orientation
    {
        get => field;
        set
        {
            if (field != value)
            {
                field = value;
                OnOrientationChanged();
                UpdateWorldMatrix();
            }
        }
    } = Quaternion<float>.Identity;

    public GameObject3D() => UpdateWorldMatrix();

    public virtual void Update(double deltaTime) { }

    public virtual void Draw(IGraphicsDevice graphicsDevice) { }

    /// <summary>
    /// Move the object in its relative orientation
    /// </summary>
    /// <param name="offset">offset == (forward, up, right)</param>
    public virtual void Move(Vector3D<float> offset)
    {
        // Add translation
        Vector3D<float> forward = Vector3D.Transform(-Vector3D<float>.UnitZ, Orientation);
        Vector3D<float> right = Vector3D.Transform(Vector3D<float>.UnitX, Orientation);

        Position += forward * offset.X;
        Position += Vector3D<float>.UnitY * offset.Y;
        Position += right * offset.Z;
    }

    /// <summary>
    /// angles = (pitch, yaw, roll)
    /// </summary>
    /// <param name="angles"></param>
    public virtual void Rotate(Vector3D<float> angles, bool allowRoll = false)
    {
        Orientation = Quaternion<float>.CreateFromAxisAngle(allowRoll
            ? Vector3D.Transform(Vector3D<float>.UnitY, Orientation) : Vector3D<float>.UnitY, angles.Y) * Orientation;
        Orientation = Quaternion<float>.CreateFromAxisAngle(
            Vector3D.Transform(Vector3D<float>.UnitX, Orientation), angles.X) * Orientation;
        Orientation = Quaternion<float>.CreateFromAxisAngle(
            Vector3D.Transform(-Vector3D<float>.UnitZ, Orientation), angles.Z) * Orientation;
    }
}

public partial class GameObject3D
{
    protected virtual void UpdateWorldMatrix()
    {
        _worldMatrix = Matrix4X4.CreateScale(Scale) *
            Matrix4X4.CreateFromQuaternion(Orientation) *
            Matrix4X4.CreateTranslation(Position);

        OnWorldMatrixChanged();
    }

    protected virtual void OnWorldMatrixChanged() { }

    protected virtual void OnOrientationChanged() { }
}
