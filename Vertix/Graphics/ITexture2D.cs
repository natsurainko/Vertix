using Silk.NET.Maths;
using System;

namespace Vertix.Graphics;

public interface ITexture2D : IDisposable
{
    bool Initialized { get; }

    Vector2D<uint> Size { get; }

    void Initialize(Vector2D<uint> size, uint format, uint mipmapLevels = 1);

    void SetData<T>(Vector2D<uint> size, Vector2D<int> offset, uint pixelFormat, uint pixelType,
        ReadOnlySpan<T> data, int mipmapLevel = 0) where T : unmanaged;

    void BindTexture(uint bindingIndex);
}