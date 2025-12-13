using System.Numerics;
using Vertix.Graphics;

namespace Vertix.Engine;

public abstract partial class GameObject3D
{
    protected Matrix4x4 _worldMatrix;

    public virtual Matrix4x4 WorldMatrix => _worldMatrix;

    public Vector3 Scale
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
    } = Vector3.One;

    public Vector3 Position
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
    } = Vector3.Zero;

    public Quaternion Orientation
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
    } = Quaternion.Identity;

    public GameObject3D() => UpdateWorldMatrix();

    public virtual void Update(double deltaTime) { }

    public virtual void Draw(IGraphicsDevice graphicsDevice) { }

    /// <summary>
    /// Move the object in its relative orientation
    /// </summary>
    /// <param name="offset">offset == (forward, up, right)</param>
    public virtual void Move(Vector3 offset)
    {
        // Add translation
        Vector3 forward = Vector3.Transform(-Vector3.UnitZ, Orientation);
        Vector3 right = Vector3.Transform(Vector3.UnitX, Orientation);

        Position += forward * offset.X;
        Position += Vector3.UnitY * offset.Y;
        Position += right * offset.Z;
    }

    /// <summary>
    /// angles = (pitch, yaw, roll)
    /// </summary>
    /// <param name="angles"></param>
    public virtual void Rotate(Vector3 angles, bool allowRoll = false)
    {
        Orientation = Quaternion.CreateFromAxisAngle(allowRoll
            ? Vector3.Transform(Vector3.UnitY, Orientation) : Vector3.UnitY, angles.Y) * Orientation;
        Orientation = Quaternion.CreateFromAxisAngle(
            Vector3.Transform(Vector3.UnitX, Orientation), angles.X) * Orientation;
        Orientation = Quaternion.CreateFromAxisAngle(
            Vector3.Transform(-Vector3.UnitZ, Orientation), angles.Z) * Orientation;
    }
}

public partial class GameObject3D
{
    protected virtual void UpdateWorldMatrix()
    {
        _worldMatrix = Matrix4x4.CreateScale(Scale) *
            Matrix4x4.CreateFromQuaternion(Orientation) *
            Matrix4x4.CreateTranslation(Position);

        OnWorldMatrixChanged();
    }

    protected virtual void OnWorldMatrixChanged() { }

    protected virtual void OnOrientationChanged() { }
}
