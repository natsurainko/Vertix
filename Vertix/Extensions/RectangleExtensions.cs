using Silk.NET.Maths;

namespace Vertix.Extensions;

public static class RectangleExtensions
{
    extension(Rectangle<float> rect)
    {
        public Matrix4X4<float> ToScreenMatrix(float translateZ = 0)
        {
            Vector2D<float> halfSize = rect.HalfSize;
            Matrix4X4<float> result = Matrix4X4<float>.Identity;

            result.M11 = halfSize.X;
            result.M22 = halfSize.Y;

            result.M41 = rect.Origin.X + halfSize.X;
            result.M42 = rect.Origin.Y + halfSize.Y;
            result.M43 = translateZ;

            return result;
        }
    }
}
