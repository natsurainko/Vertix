using Silk.NET.Maths;

namespace Vertix.Graphics.Resources;

public record struct Vertex2D()
{
    public Vector2D<float> Position;

    public uint ZIndex = 0;

    public Vector4D<float> Color = Vector4D<float>.One;

    public readonly static VertexArrayProperty[] DefaultProperties =
    [
        new VertexArrayProperty<float>(2, 0),
        new VertexArrayProperty<uint>(1, 1),
        new VertexArrayProperty<float>(4, 2)
    ];

    public struct InstanceTransform2D()
    {
        public Vector2D<float> Position;

        public Vector2D<float> Size;

        public uint ZIndex = 0;

        public Vector4D<float> Color = Vector4D<float>.One;

        public Vector2D<float> TextureOffset = Vector2D<float>.Zero;

        public Vector2D<float> TextureScale = Vector2D<float>.One;

        public readonly static VertexArrayProperty[] DefaultProperties =
        [
            new VertexArrayProperty<float>(2, 3),
            new VertexArrayProperty<float>(2, 4),
            new VertexArrayProperty<uint>(1, 1),
            new VertexArrayProperty<float>(4, 2),
            new VertexArrayProperty<float>(2, 5),
            new VertexArrayProperty<float>(2, 6)
        ];
    }
}