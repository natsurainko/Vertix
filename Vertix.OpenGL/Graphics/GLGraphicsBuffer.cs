using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;

namespace Vertix.OpenGL.Graphics;

public partial class GLGraphicsBuffer(GL gL, uint handle, GraphicsBufferUsage bufferUsage, GraphicsBufferMapAccess bufferMapAccess = GraphicsBufferMapAccess.None) : IGraphicsBuffer
{
    private readonly GL _gL = gL;
    private readonly uint _handle = handle;
    private BufferStorageMask _storageFlags = BufferStorageMask.None;

    public uint Handle => _handle;

    public bool Initialized { get; private set; }

    public GraphicsBufferUsage BufferUsage { get; } = bufferUsage;

    public GraphicsBufferMapAccess BufferMapAccess { get; } = bufferMapAccess;

    public GLGraphicsBuffer(GL gL, GraphicsBufferUsage bufferUsage, GraphicsBufferMapAccess bufferMapAccess = GraphicsBufferMapAccess.None)
        : this(gL, gL.CreateBuffer(), bufferUsage, bufferMapAccess) { }

    public unsafe void Initialize<T>(int length, bool fillable = false, ReadOnlySpan<T> data = default)
        where T : unmanaged
    {
        if (!fillable && BufferMapAccess == GraphicsBufferMapAccess.None)
            _storageFlags = BufferStorageMask.None;
        else if (fillable)
            _storageFlags |= BufferStorageMask.DynamicStorageBit;

        if (BufferMapAccess != GraphicsBufferMapAccess.None)
        {
            if (fillable) throw new InvalidOperationException();

            if ((BufferMapAccess & GraphicsBufferMapAccess.Read) == GraphicsBufferMapAccess.Read)
                _storageFlags |= BufferStorageMask.MapReadBit;
            if ((BufferMapAccess & GraphicsBufferMapAccess.Write) == GraphicsBufferMapAccess.Write)
                _storageFlags |= BufferStorageMask.MapWriteBit;
        }

        _gL.NamedBufferStorage(_handle, (nuint)(length * sizeof(T)), data, _storageFlags);
        Initialized = true;
    }

    public unsafe void Fill<T>(nint offset, int length, ReadOnlySpan<T> data) where T : unmanaged
    {
        if (!Initialized || ((_storageFlags & BufferStorageMask.DynamicStorageBit) != BufferStorageMask.DynamicStorageBit))
            throw new InvalidOperationException("Buffer is not initialized with dynamic storage flag.");

        _gL.NamedBufferSubData(_handle, offset, (uint)(length * sizeof(T)), data[..length]);
    }

    public void BindAsUniform(uint bindingIndex)
    {
        if ((BufferUsage & GraphicsBufferUsage.UniformBuffer) != GraphicsBufferUsage.UniformBuffer)
            throw new InvalidOperationException("The provided buffer is not initialized as an uniform buffer.");

        _gL.BindBufferBase(BufferTargetARB.UniformBuffer, bindingIndex, _handle);
    }

    public void Dispose() => _gL.DeleteBuffer(_handle);
}