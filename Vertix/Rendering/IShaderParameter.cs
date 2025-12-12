using System;

namespace Vertix.Rendering;

public interface IShaderParameter
{
    string Name { get; }

    void SetValue<T>(T value) where T : unmanaged;

    void SetValues<T>(ReadOnlySpan<T> values, uint length) where T : unmanaged;
}
