using Silk.NET.Maths;
using System.Numerics;

namespace Vertix.Graphics.Text;

public struct FontGlyph
{
    public Vector2D<uint> Size;

    public Vector2 Bearing;

    public float Advance;

    public Vector2 UVTopLeft;

    public Vector2 UVBottomRight;
}
