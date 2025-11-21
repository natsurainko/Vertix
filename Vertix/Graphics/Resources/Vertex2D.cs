using Silk.NET.Maths;

namespace Vertix.Graphics.Resources;

public record struct Vertex2D
{
    public Vector2D<float> Position;

    public uint ZIndex;

    public Vector4D<float> Color;

    public readonly static VertexArrayProperty[] DefaultProperties =
    [
        new VertexArrayProperty<float>(2, 0),
        new VertexArrayProperty<uint>(1, 1),
        new VertexArrayProperty<float>(4, 2)
    ];

    public struct InstanceTransform2D
    {
        public Vector2D<float> Position;

        public Vector2D<float> Size;

        public uint ZIndex;

        public Vector4D<float> Color;

        public readonly static VertexArrayProperty[] DefaultProperties =
        [
            new VertexArrayProperty<float>(2, 3),
            new VertexArrayProperty<float>(2, 4),
            new VertexArrayProperty<uint>(1, 1),
            new VertexArrayProperty<float>(4, 2)
        ];
    }
}