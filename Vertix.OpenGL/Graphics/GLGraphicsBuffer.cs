using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;

namespace Vertix.OpenGL.Graphics;

public struct GLGraphicsBuffer : IGraphicsBuffer
{
    private readonly GL _gL;
    private readonly uint _handle;
    private BufferStorageMask _storageFlags;

    public readonly uint Handle => _handle;

    public bool Initialized { get; private set; }

    public GLGraphicsBuffer(GL gL)
    {
        _gL = gL;
        _handle = _gL.CreateBuffer();
    }

    public GLGraphicsBuffer(GL gL, uint handle)
    {
        _gL = gL;
        _handle = handle;
    }

    public unsafe void Initialize<T>(int length, uint flags, ReadOnlySpan<T> data) where T : unmanaged
    {
        _storageFlags = (BufferStorageMask)flags;
        _gL.NamedBufferStorage(_handle, (nuint)(length * sizeof(T)), data, _storageFlags);

        this.Initialized = true;
    }

    public readonly unsafe void Fill<T>(nint offset, int length, ReadOnlySpan<T> data) where T : unmanaged
    {
        if (!Initialized || ((_storageFlags & BufferStorageMask.DynamicStorageBit) != BufferStorageMask.DynamicStorageBit))
            throw new InvalidOperationException("Buffer is not initialized with dynamic storage flag.");

        _gL.NamedBufferSubData(_handle, offset, (uint)(length * sizeof(T)), data[..length]);
    }

    public void Dispose()
    {
        _gL.DeleteBuffer(_handle);
    }
}
