namespace Vertix.Graphics.Primitives;

public record struct Mesh
{
    public Vertex[] Vertices { get; set; }

    public uint[] Indices { get; set; }

    public string Name { get; set; }

    public IVertexArray? VertexArray;
}
