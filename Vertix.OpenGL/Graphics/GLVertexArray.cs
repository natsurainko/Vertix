using Silk.NET.OpenGL;
using System;
using System.Collections.Generic;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;

namespace Vertix.OpenGL.Graphics;

public record struct GLVertexArray : IVertexArray
{
    private readonly GL _gL;
    private readonly uint _handle;
    private readonly Dictionary<uint, IGraphicsBuffer> _attachedBuffers = [];

    public readonly uint Handle => _handle;

    public IGraphicsBuffer? VertexBuffer { get; private set; }

    public IGraphicsBuffer? IndexBuffer { get; private set; }

    public bool Initialized { get; private set; }

    public readonly IReadOnlyDictionary<uint, IGraphicsBuffer> AttachedBuffers => _attachedBuffers;

    public GLVertexArray(GL gl)
    {
        _gL = gl; 
        _handle = gl.CreateVertexArray();
    }

    public GLVertexArray(GL gL, uint handle)
    {
        _gL = gL;
        _handle = handle;
    }

    public unsafe void Initialize<T>(in IGraphicsBuffer vertexBuffer, ReadOnlySpan<VertexArrayProperty> properties, 
        in IGraphicsBuffer? indexBuffer = null, nint vertexBufferOffset = 0) where T : unmanaged
    {
        if (vertexBuffer is not GLGraphicsBuffer gLVertexBuffer) throw new InvalidOperationException();
        GLGraphicsBuffer? gLIndexBuffer = indexBuffer as GLGraphicsBuffer?;

        this.VertexBuffer = vertexBuffer;
        this.IndexBuffer = indexBuffer;
        this.Initialized = true;

        _attachedBuffers[0] = gLVertexBuffer;
        _gL.VertexArrayVertexBuffer(_handle, 0, gLVertexBuffer.Handle, vertexBufferOffset, (uint)sizeof(T));

        if (gLIndexBuffer != null)
            _gL.VertexArrayElementBuffer(_handle, gLIndexBuffer.Value.Handle);

        uint offset = 0;

        for (int i = 0; i < properties.Length; i++)
        {
            VertexArrayProperty property = properties[i];
            VertexAttribType? baseType = EnumHelper.GetVertexAttribType(property.TypeName)
                ?? throw new InvalidOperationException();

            _gL.VertexArrayAttribFormat(_handle, property.Location, property.Count, baseType.Value, property.Normalized, offset);
            _gL.VertexArrayAttribBinding(_handle, property.Location, 0);
            _gL.EnableVertexArrayAttrib(_handle, property.Location);

            offset += property.Size;
        }
    }

    public readonly unsafe void AttachInstanceBuffer<T>(in IGraphicsBuffer instanceBuffer,
        ReadOnlySpan<VertexArrayProperty> properties, uint instancedDivisor = 1, nint bufferOffset = 0, uint? targetBindingIndex = null) where T : unmanaged
    {
        if (instanceBuffer is not GLGraphicsBuffer gLGraphicsBuffer) throw new InvalidOperationException();
        if (instancedDivisor < 1) throw new ArgumentException("The instancedDivisor must be at least 1.");

        uint bindingIndex = targetBindingIndex ?? GetAvailableBindingIndex();
        _attachedBuffers[bindingIndex] = gLGraphicsBuffer;
        _gL.VertexArrayVertexBuffer(_handle, bindingIndex, gLGraphicsBuffer.Handle, bufferOffset, (uint)sizeof(T));

        uint offset = 0;

        for (int i = 0; i < properties.Length; i++)
        {
            VertexArrayProperty property = properties[i];
            VertexAttribType? baseType = EnumHelper.GetVertexAttribType(property.TypeName)
                ?? throw new InvalidOperationException();

            _gL.VertexArrayAttribFormat(_handle, property.Location, property.Count, baseType.Value, property.Normalized, offset);
            _gL.VertexArrayAttribBinding(_handle, property.Location, bindingIndex);
            _gL.EnableVertexArrayAttrib(_handle, property.Location);

            offset += property.Size;
        }

        _gL.VertexArrayBindingDivisor(_handle, bindingIndex, instancedDivisor);
    }

    public readonly void DetachBuffer(in IGraphicsBuffer instanceBuffer)
    {
        foreach ((uint bindingIndex, IGraphicsBuffer buffer) in _attachedBuffers)
        {
            if (buffer == instanceBuffer)
            {
                _attachedBuffers.Remove(bindingIndex);
                _gL.VertexArrayVertexBuffer(_handle, bindingIndex, 0, IntPtr.Zero, 0);
                break;
            }
        }
    }

    public readonly void Bind() => _gL.BindVertexArray(_handle);

    public readonly void Dispose()
    {
        VertexBuffer?.Dispose();
        IndexBuffer?.Dispose();

        _gL?.DeleteVertexArray(_handle);
    }

    private readonly uint GetAvailableBindingIndex()
    {
        uint start = 1;

        while (_attachedBuffers.ContainsKey(start))
            start++;

        return start;
    }
}