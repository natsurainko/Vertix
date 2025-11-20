namespace Vertix.Rendering;

public interface IShaderParameter
{
    string Name { get; }

    void SetValue<T>(T value) where T : unmanaged;
}
