using System;

namespace Vertix.Graphics;

public interface IGraphicsBuffer : IDisposable
{
    bool Initialized { get; }

    void Initialize<T>(int length, uint flags, ReadOnlySpan<T> data) where T : unmanaged;

    void Fill<T>(nint offset, int length, ReadOnlySpan<T> data) where T : unmanaged;
}