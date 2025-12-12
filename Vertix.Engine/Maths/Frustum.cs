using Silk.NET.Maths;
using System;

namespace Vertix.Engine.Maths;

public struct Frustum<T> where T : unmanaged, IFormattable, IEquatable<T>, IComparable<T>
{
    public Plane<T> LeftPlane { get; set; }
    public Plane<T> RightPlane { get; set; }

    public Plane<T> TopPlane { get; set; }
    public Plane<T> BottomPlane { get; set; }

    public Plane<T> NearPlane { get; set; }
    public Plane<T> FarPlane { get; set; }
}
