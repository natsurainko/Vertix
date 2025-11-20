using System;
using System.Collections.Generic;

namespace Vertix.Graphics;

public interface IVertexArray : IDisposable
{
    IGraphicsBuffer? VertexBuffer { get; }

    IGraphicsBuffer? IndexBuffer { get; }

    bool Initialized { get; }

    IReadOnlyDictionary<uint, IGraphicsBuffer> AttachedBuffers { get; }

    void Initialize<T>(in IGraphicsBuffer vertexBuffer, ReadOnlySpan<VertexArrayProperty> properties, 
        in IGraphicsBuffer? indexBuffer = null, nint vertexBufferOffset = 0) where T : unmanaged;

    void AttachInstanceBuffer<T>(in IGraphicsBuffer instanceBuffer, ReadOnlySpan<VertexArrayProperty> properties,
        uint instancedDivisor = 1, nint bufferOffset = 0, uint? targetBindingIndex = null) where T : unmanaged;

    void DetachBuffer(in IGraphicsBuffer instanceBuffer);

    void Bind();
}
