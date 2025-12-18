using Plane = System.Numerics.Plane;

namespace Vertix.Engine.Camera;

public struct Frustum
{
    public Plane LeftPlane { get; set; }
    public Plane RightPlane { get; set; }

    public Plane TopPlane { get; set; }
    public Plane BottomPlane { get; set; }

    public Plane NearPlane { get; set; }
    public Plane FarPlane { get; set; }
}
