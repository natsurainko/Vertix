namespace Vertix.Graphics.Primitives;

public record class Model
{
    public string? Name { get; set; }

    public Mesh[] Meshes { get; set; } = [];

    public void Draw(IGraphicsDevice graphicsDevice)
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

    public void InitializeMeshesVertexArray(IGraphicsDevice graphicsDevice)
    {
        for (int i = 0; i < Meshes.Length; i++)
        {
            IGraphicsBuffer vertexBuffer = graphicsDevice.CreateGraphicsBuffer(GraphicsBufferUsage.VertexBuffer);
            IGraphicsBuffer indexBuffer = graphicsDevice.CreateGraphicsBuffer(GraphicsBufferUsage.IndexBuffer);
            IVertexArray vertexArray = graphicsDevice.CreateVertexArray();

            vertexBuffer.Initialize(Meshes[i].Vertices.Length, data: Meshes[i].Vertices);
            indexBuffer.Initialize(Meshes[i].Indices.Length, data: Meshes[i].Indices);
            vertexArray.Initialize<Vertex>(vertexBuffer, Vertex.DefaultProperties, indexBuffer);

            Meshes[i].VertexArray = vertexArray;
        }
    }

    public void DisposeMeshesVertexArray()
    {
        for (int i = 0; i < Meshes.Length; i++)
        {
            Meshes[i].VertexArray?.Dispose();
            Meshes[i].VertexArray = null;
        }
    }
}
