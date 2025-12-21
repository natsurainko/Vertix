using System;

namespace Vertix.Graphics;

public interface IGraphicsBuffer : IDisposable
{
    bool Initialized { get; }

    GraphicsBufferUsage BufferUsage { get; }

    GraphicsBufferMapAccess BufferMapAccess { get; }

    void Initialize<T>(int length, bool fillable = false, ReadOnlySpan<T> data = default)
        where T : unmanaged;

    void Fill<T>(nint offset, int length, ReadOnlySpan<T> data) where T : unmanaged;

    void BindAsUniform(uint bindingIndex);
}

public enum GraphicsBufferUsage : uint
{
    None = 0x0,

    // Common Naming
    VertexBuffer = 0x1,
    IndexBuffer = 0x2,
    IndirectBuffer = 0x3,

    // OpenGL Naming
    UniformBuffer = 0x4,

    // D3D11 Naming
    ConstantBuffer = UniformBuffer,
}

public enum GraphicsBufferMapAccess : uint
{
    None = 0x0,
    Read = 0x1,
    Write = 0x2,
}