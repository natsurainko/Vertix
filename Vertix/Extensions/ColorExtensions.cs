using System.Drawing;
using System.Numerics;

namespace Vertix.Extensions;

public static class ColorExtensions
{
    extension(Color color)
    {
        public Vector4 ToVector4() => new(color.R / 255f, color.G / 255f, color.B / 255f, color.A / 255f);
    }
}
