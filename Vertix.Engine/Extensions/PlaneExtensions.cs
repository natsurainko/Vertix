using Silk.NET.Maths;
using System;

namespace Vertix.Engine.Extensions;

public static class PlaneExtensions
{
    extension(Plane)
    {
        public static Plane<T> Create<T>(Vector3D<T> position, Vector3D<T> normal)
            where T : unmanaged, IEquatable<T>, IFormattable, IComparable<T>
        {
            return new Plane<T>()
            {
                Distance = Vector3D.Dot(normal, position),
                Normal = Vector3D.Normalize(normal)
            };
        }
    }
}
