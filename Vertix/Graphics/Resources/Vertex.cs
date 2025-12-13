using System.Numerics;

namespace Vertix.Graphics.Resources;

public record struct Vertex()
{
    public Vector3 Position;

    public Vector3 Normal;

    public Vector2 TextureCoord;

    public Vector4 TextureRegion = new(0, 0, 1, 1);

    public readonly static VertexArrayProperty[] DefaultProperties =
    [
        new VertexArrayProperty<float>(3, 0),
        new VertexArrayProperty<float>(3, 1),
        new VertexArrayProperty<float>(2, 2),
        new VertexArrayProperty<float>(4, 3),
    ];

    public struct InstanceTransform3D()
    {
        public Matrix4x4 WorldMatirx;

        public Vector4 TextureRegion = new(0, 0, 1, 1);

        public readonly static VertexArrayProperty[] DefaultProperties =
        [
            new VertexArrayProperty<float>(4, 4),
            new VertexArrayProperty<float>(4, 5),
            new VertexArrayProperty<float>(4, 6),
            new VertexArrayProperty<float>(4, 7),

            new VertexArrayProperty<float>(4, 3),
        ];
    }
}