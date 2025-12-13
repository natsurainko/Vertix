using System.Numerics;
using Plane = System.Numerics.Plane;

namespace Vertix.Engine.Extensions;

public static class PlaneExtensions
{
    extension(Plane)
    {
        public static Plane Create(Vector3 position, Vector3 normal)
        {
            return new Plane()
            {
                D = Vector3.Dot(normal, position),
                Normal = Vector3.Normalize(normal)
            };
        }
    }
}
