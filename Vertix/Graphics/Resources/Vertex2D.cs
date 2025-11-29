using Silk.NET.Maths;

namespace Vertix.Graphics.Resources;

public record struct Vertex2D()
{
    public Vector3D<float> Position;

    public Vector4D<float> Color = Vector4D<float>.One;

    public Vector2D<float> TextureCoord;

    public Vector4D<float> TextureRegion = new(0, 0, 1, 1);

    public readonly static VertexArrayProperty[] DefaultProperties =
    [
        new VertexArrayProperty<float>(3, 0),
        new VertexArrayProperty<float>(4, 1),
        new VertexArrayProperty<float>(2, 2),
        new VertexArrayProperty<float>(4, 3),
    ];

    public struct InstanceTransform2D()
    {
        public Matrix4X4<float> WorldMatirx;

        public Vector4D<float> Color = Vector4D<float>.One;

        public Vector4D<float> TextureRegion = new(0, 0, 1, 1);

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
}