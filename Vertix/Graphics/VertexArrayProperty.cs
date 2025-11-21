namespace Vertix.Graphics;

public abstract record class VertexArrayProperty
{
    public int Count { get; }

    public uint Location { get; }

    public bool Normalized { get; } = false;

    public abstract uint Size { get; }

    public abstract string TypeName { get; }

    public VertexArrayProperty(int count, uint location, bool normalized = false)
    {
        Count = count;
        Location = location;
        Normalized = normalized;
    }
}

public record class VertexArrayProperty<T> : VertexArrayProperty where T : unmanaged
{
    public unsafe override uint Size { get; } 

    public override string TypeName { get; }

    public unsafe VertexArrayProperty(int c, uint l, bool n = false) : base(c, l, n)
    {
        Size = (uint)(sizeof(T) * c);
        TypeName = typeof(T).FullName ?? typeof(T).Name;
    }
}