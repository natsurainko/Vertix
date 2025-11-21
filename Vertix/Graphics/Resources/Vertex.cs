using Silk.NET.Maths;

namespace Vertix.Graphics.Resources;

public record struct Vertex
{
    public Vector3D<float> Position;

    public Vector3D<float> Normal;

    public Vector2D<float> TexCoord;

    public readonly static VertexArrayProperty[] DefaultProperties =
    [
        new VertexArrayProperty<float>(3, 0),
        new VertexArrayProperty<float>(3, 1),
        new VertexArrayProperty<float>(2, 2)
    ];

    public struct InstanceTransform3D
    {
        public Matrix4X4<float> WorldMatirx;

        public readonly static VertexArrayProperty[] DefaultProperties =
        [
            new VertexArrayProperty<float>(4, 3),
            new VertexArrayProperty<float>(4, 4),
            new VertexArrayProperty<float>(4, 5),
            new VertexArrayProperty<float>(4, 6),
        ];
    }
}