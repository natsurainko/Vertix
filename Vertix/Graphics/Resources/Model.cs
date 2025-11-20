namespace Vertix.Graphics.Resources;

public record struct Model
{
    public string Name { get; set; }

    public Mesh[] Meshes { get; set; }

    public readonly void Draw(IGraphicsDevice graphicsDevice)
    {
        for (int i = 0; i < Meshes.Length; i++)
        {
            Mesh mesh = Meshes[i];
            if (mesh.VertexArray == null) continue;
            graphicsDevice.DrawVertexElementsArray
            (
                mesh.VertexArray,
                PrimitiveType.Triangles, 
                (uint)mesh.Indices.Length
            );
        }
    }
}
