using Silk.NET.Maths;

namespace Vertix.Graphics.Text;

public struct FontGlyph
{
    public Vector2D<uint> Size;

    public Vector2D<float> Bearing;

    public float Advance;

    public Vector2D<float> UVTopLeft;

    public Vector2D<float> UVBottomRight;
}
