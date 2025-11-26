using Silk.NET.Maths;
using System;

namespace Vertix.Graphics;

public interface ITexture : IDisposable
{
    uint MipmapLevels { get; }

    bool Initialized { get; }

    TextureFormat TextureFormat { get; }

    void BindTexture(uint bindingIndex);
}

public interface ITexture2D : ITexture
{
    Vector2D<uint> Size { get; }

    void Initialize(Vector2D<uint> size, TextureFormat format, uint mipmapLevels = 1);

    void SetData<TData>(Vector2D<uint> size, Vector2D<int> offset, ReadOnlySpan<TData> data, int mipmapLevel = 0) 
        where TData : unmanaged;
}