using System.Numerics;

namespace Vertix.Graphics.Primitives;

public record struct Vertex2D()
{
    public Vector3 Position;

    public Vector4 Color = Vector4.One;

    public Vector2 TextureCoord;

    public Vector4 TextureRegion = new(0, 0, 1, 1);

    public readonly static VertexArrayProperty[] DefaultProperties =
    [
        new VertexArrayProperty<float>(3, 0),
        new VertexArrayProperty<float>(4, 1),
        new VertexArrayProperty<float>(2, 2),
        new VertexArrayProperty<float>(4, 3),
    ];

    public struct InstanceTransform2D()
    {
        public Matrix4x4 WorldMatirx;

        public Vector4 Color = Vector4.One;

        public Vector4 TextureRegion = new(0, 0, 1, 1);

        public readonly static VertexArrayProperty[] DefaultProperties =
        [
            new VertexArrayProperty<float>(4, 4),
            new VertexArrayProperty<float>(4, 5),
            new VertexArrayProperty<float>(4, 6),
            new VertexArrayProperty<float>(4, 7),

            new VertexArrayProperty<float>(4, 1),
            new VertexArrayProperty<float>(4, 3),
        ];
    }

    public static readonly Vertex2D[] QuadVertices =
    [
        new() { Position = new(-1, 1, 0), TextureCoord = new(0, 1) },
        new() { Position = new(-1, -1, 0), TextureCoord = new(0, 0) },
        new() { Position = new(1, 1, 0), TextureCoord = new(1, 1) },
        new() { Position = new(1, -1, 0), TextureCoord = new(1, 0) },
    ];
}