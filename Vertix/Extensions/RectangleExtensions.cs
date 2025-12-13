using Silk.NET.Maths;
using System.Numerics;

namespace Vertix.Extensions;

public static class RectangleExtensions
{
    extension(Rectangle<float> rect)
    {
        public Matrix4x4 ToScreenMatrix(float translateZ = 0)
        {
            Vector2D<float> halfSize = rect.HalfSize;
            Matrix4x4 result = Matrix4x4.Identity;

            result.M11 = halfSize.X;
            result.M22 = halfSize.Y;

            result.M41 = rect.Origin.X + halfSize.X;
            result.M42 = rect.Origin.Y + halfSize.Y;
            result.M43 = translateZ;

            return result;
        }
    }
}
