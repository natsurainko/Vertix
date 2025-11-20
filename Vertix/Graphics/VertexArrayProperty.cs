namespace Vertix.Graphics;

public abstract record class VertexArrayProperty
{
    public int Count { get; }

    public uint Location { get; }

    public abstract uint Size { get; }

    public abstract string TypeName { get; }

    public VertexArrayProperty(int count, uint location)
    {
        Count = count;
        Location = location;
    }
}

public record class VertexArrayProperty<T> : VertexArrayProperty where T : unmanaged
{
    public unsafe override uint Size { get; } 

    public override string TypeName { get; }

    public unsafe VertexArrayProperty(int c, uint l) : base(c, l)
    {
        Size = (uint)(sizeof(T) * c);
        TypeName = typeof(T).FullName ?? typeof(T).Name;
    }
}